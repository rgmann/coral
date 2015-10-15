// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#include <sstream>
#include <iostream>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <boost/thread/locks.hpp>
#include "Log.h"
#include "Date.h"

#define MAX_MESSAGE_LEN_BYTES  (512)

using namespace coral::log;
using namespace coral::thread;

const char* coral::log::log_level_to_string( LogLevel level )
{
   // Note:
   // The following array must be manually synchronized with the LogLevel
   // enumeration.
   static const char* LOG_LEVELS[] = {
      "",         // Suppress
      "",         // Raw
      "Status",   // Status
      "Error",    // Error
      "Warn",     // Warn
      "Debug",    // Debug
      "MemDump",  // MemDump
      "Verbose",  // Verbose
      "Unknown"
   };

   if ( ( level >= Supress ) && ( level < InvalidLogLevel ) )
   {
      return LOG_LEVELS[ level ];
   }

   return LOG_LEVELS[ InvalidLogLevel ];
};

//-----------------------------------------------------------------------------
LogMessage::LogMessage()
   : log_level_( InvalidLogLevel )
   , timestamp_( Timestamp::Now() )
   , is_flush_marker_( true )
{
}

//-----------------------------------------------------------------------------
LogMessage::LogMessage(LogLevel level, const std::string& message)
   : log_level_( level )
   , message_  ( message )
   , timestamp_( Timestamp::Now() )
   , is_flush_marker_( false )
{
}

//-----------------------------------------------------------------------------
LogMessage::LogMessage(
   LogLevel    level,
   const char* header,
   const char* data_ptr,
   ui32        length,
   ui32        max_line_length
)
   : log_level_( level )
   , timestamp_( Timestamp::Now() )
   , is_flush_marker_( false )
{
   char row_prefix[7];
   std::stringstream stream;

   if ( strlen( header ) > 0 )
   {
      stream << header << "\n";
   }

   ui32 offset = 0;
   for (; offset < length; ++offset )
   {
      if ( ( offset % max_line_length ) == 0)
      {
         snprintf(row_prefix, sizeof( row_prefix ), "%4X: ", offset);
         stream << row_prefix;
      }

      snprintf( row_prefix, sizeof(row_prefix), "%02X ", (unsigned char)data_ptr[ offset ] );
      stream << row_prefix;

      if ( ( offset > 0 ) && ( ( offset + 1 ) % max_line_length == 0 ) )
      {
         stream << "\n";
      }
   }

   if ( offset > 0 )
   {
      stream << "\n";
   }

   message_.assign( stream.str() );
}

//-----------------------------------------------------------------------------
std::string LogMessage::toString( ui32 format ) const
{
   std::stringstream ss;

   if ( format & log::DisplayTimestamp )
   {
      ss << std::fixed << timestamp_.fseconds() << std::dec << " ";
   }

   if ( format & log::DisplayLogLevel )
   {
      ss << log_level_to_string( log_level_ ) ;

      if ( log_level_ != Raw )
      {
         ss << ":";
      }

      ss << " ";
   }

   ss << message_;

   return ss.str();
}

//-----------------------------------------------------------------------------
void LogMessage::signalFlushed()
{
   if ( is_flush_marker_ )
   {
      flush_signal_.give();
   }
}

//-----------------------------------------------------------------------------
bool LogMessage::waitFlush( i32 timeout_ms )
{
   bool flush_success = false;

   if ( is_flush_marker_ )
   {
      flush_success =
         ( flush_signal_.take( timeout_ms ) == Semaphore::SemAcquired );
   }

   return flush_success;
}


//-----------------------------------------------------------------------------
Logger::Logger()
   : coral::concurrency::IThread("CommonLoggingThread")
   , console_log_options_  ( log::DisplayLogLevel )
   , log_level_            ( coral::log::Status )
   , log_file_enabled_     ( false )
   , path_                 (boost::filesystem::current_path().generic_string())
   , suffix_               ( "" )
   , current_log_file_size_( 0 )
{
   launch();
}

//-----------------------------------------------------------------------------
Logger::~Logger()
{
   cancel( true );
}

//-----------------------------------------------------------------------------
void Logger::setLogFileEnabled( bool enable )
{
   log_file_enabled_ = enable;
}

//-----------------------------------------------------------------------------
void Logger::setPath( const std::string& path )
{
   boost::mutex::scoped_lock guard( file_attribute_lock_ );
   path_ = path;
}

//-----------------------------------------------------------------------------
void Logger::setSuffix( const std::string& suffix )
{
   boost::mutex::scoped_lock guard( file_attribute_lock_ );
   suffix_ = suffix;
}

//-----------------------------------------------------------------------------
void Logger::setFilterLevel( LogLevel level )
{
   log_level_ = level;
}

//-----------------------------------------------------------------------------
void Logger::setConsoleDisplayOptions( ui32 display_options )
{
   console_log_options_ = display_options;
}

//-----------------------------------------------------------------------------
void Logger::send( LogMessagePtr message_ptr )
{
   messages_.push( message_ptr, 0 );
}

//-----------------------------------------------------------------------------
void Logger::run( const bool& shutdown )
{   
   LogMessagePtr message;
   std::string path = path_;
   std::string suffix = suffix_;

   std::ofstream log_file;

   while ( shutdown == false )
   {
      if ( messages_.pop( message ) && message )
      {
         if( message->is_flush_marker_ )
         {
            message->signalFlushed();
         }
         else
         {
            if ( message->log_level_ <= log_level_ )
            {
               std::cout << message->toString( console_log_options_ );
            }

            if ( log_file_enabled_ )
            {
               {
                  boost::mutex::scoped_lock guard( file_attribute_lock_ );
                  path   = path_;
                  suffix = suffix_;
               }

               if ( log_file.is_open() == false )
               {
                  log_file.close();

                  // openLogFile( path, suffix );
                  std::string full_log_filename = generateLogName( path, suffix );

                  log_file.open( full_log_filename.c_str(), std::ofstream::out );
               }

               if ( log_file.is_open() )
               {
                  std::string log_message = message->toString( log::DisplayAll );
                  current_log_file_size_ += log_message.size();
                  log_file << log_message;
               }
            }
            else
            {
               if ( log_file.is_open() )
               {
                  log_file.close();
               }
            }
         }
      }
   }

   // Make sure the log file is closed.
   log_file.close();
}

//-----------------------------------------------------------------------------
std::string Logger::generateLogName(
   const std::string& path,
   const std::string& suffix
)
{
   Date log_date;

   log_date.sample();
   std::stringstream ss;
   ss << log_date.year() << "_";
   ss << (log_date.month() + 1) << "_";
   ss << log_date.dayOfMonth() << "_";
   ss << log_date.hour() << "_";
   ss << log_date.minute() << "_";
   ss << log_date.second() << "_";

   if ( suffix.empty() == false )
   {
      ss << suffix << "_";
   }
   ss << "log.txt";

   current_log_file_size_ = 0;

   boost::filesystem::path log_path( path );
   log_path /= ss.str();

   return log_path.string();
}

Logger coral::log::glog;

//-----------------------------------------------------------------------------
void coral::log::set_path( const std::string& path )
{
   coral::log::glog.setPath( path );
}

//-----------------------------------------------------------------------------
void coral::log::set_suffix(const std::string& suffix)
{
   coral::log::glog.setSuffix( suffix );
}

//-----------------------------------------------------------------------------
void coral::log::enable()
{
   coral::log::glog.setLogFileEnabled(true);
}

//-----------------------------------------------------------------------------
void coral::log::disable()
{
   coral::log::glog.setLogFileEnabled(false);
}

//-----------------------------------------------------------------------------
void coral::log::options(ui32 opts)
{
   coral::log::glog.setConsoleDisplayOptions(opts);
}

//-----------------------------------------------------------------------------
void coral::log::level(LogLevel level)
{
   coral::log::glog.setFilterLevel(level);
}

//-----------------------------------------------------------------------------
void coral::log::flush( i32 timeout_ms )
{
   LogMessagePtr flush_message_ptr( new LogMessage() );

   coral::log::glog.send( flush_message_ptr );
   flush_message_ptr->waitFlush( timeout_ms );
}

//-----------------------------------------------------------------------------
void coral::log::print(LogLevel level, const char* format, va_list args)
{
   static char message_buffer[ MAX_MESSAGE_LEN_BYTES ];

   if ( format )
   {
      vsnprintf( message_buffer, sizeof( message_buffer ), format, args );

      LogMessagePtr log_message_ptr( new LogMessage(
         level, std::string( message_buffer )
      ));

      coral::log::glog.send( log_message_ptr );
   }
}

//-----------------------------------------------------------------------------
void coral::log::raw( const char* format, ... )
{
   if (format)
   {
      va_list args;
      va_start( args, format );
      coral::log::print( Raw, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void coral::log::status( const char* format, ... )
{
   if (format)
   {
      va_list args;
      va_start( args, format );
      coral::log::print( Status, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void coral::log::error( const char* format, ... )
{
   if ( format )
   {
      va_list args;
      va_start( args, format );
      coral::log::print( Error, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void coral::log::warn( const char* format, ... )
{
   if ( format )
   {
      va_list args;
      va_start( args, format );
      coral::log::print( Warn, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void coral::log::debug( const char* format, ... )
{
   if ( format )
   {
      va_list args;
      va_start( args, format );
      coral::log::print( Debug, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void coral::log::mem_dump(
   const char* header,
   const char* data_ptr,
   ui32        length,
   ui32        max_line_length )
{
   if ( ( header != NULL ) && ( data_ptr != NULL ) )
   {
      try
      {
         LogMessagePtr log_message_ptr( new LogMessage(
            MemDump,
            header,
            data_ptr,
            length,
            max_line_length
         ));

         coral::log::glog.send( log_message_ptr );
      }
      catch ( ... )
      {
         coral::log::error(
            "coral::log::mem_dump - exception while logging dump\n" );
      }
   }
}


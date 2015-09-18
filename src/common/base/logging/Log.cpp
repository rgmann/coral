#include <sstream>
#include <iostream>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <boost/thread/locks.hpp>
#include "Log.h"
#include "Date.h"

#define MAX_MESSAGE_LEN_BYTES  (512)

using namespace liber::log;

//-----------------------------------------------------------------------------
LogMessage::LogMessage(LogLevel level, const std::string& message)
   : log_level_( level )
   , message_  ( message )
   , timestamp_( Timestamp::Now() )
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
      ss << LogLevelToString( log_level_ ) ;

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
Logger::Logger()
   : liber::concurrency::IThread("CommonLoggingThread")
   , console_log_options_  ( log::DisplayLogLevel )
   , log_level_            ( liber::log::Status )
   , allow_messages_       ( true )
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
  log_file_.close();
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
void Logger::send( const LogMessage& message )
{
   if ( allow_messages_ )
   {
      messages_.push( message, 0 );
   }
}

//-----------------------------------------------------------------------------
void Logger::flush()
{
   allow_messages_ = false;

   while ( messages_.size() > 0 )
   {
      usleep( 100000 );
   }

   allow_messages_ = true;
}

//-----------------------------------------------------------------------------
void Logger::run( const bool& shutdown )
{
   LogMessage message;
   std::string path = path_;
   std::string suffix = suffix_;

   while ( shutdown == false )
   {
      if ( messages_.pop( message ) )
      {
         if ( message.log_level_ <= log_level_ )
         {
            std::cout << message.toString(console_log_options_);
         }

         if ( log_file_enabled_ )
         {
            {
               boost::mutex::scoped_lock guard( file_attribute_lock_ );
               path   = path_;
               suffix = suffix_;
            }

            if ( log_file_.is_open() == false )
            {
               log_file_.close();
               openLogFile( path, suffix );
            }

            if ( log_file_.is_open() )
            {
               std::string log_message = message.toString( log::DisplayAll );
               current_log_file_size_ += log_message.size();
               log_file_ << log_message;
            }
         }
         else
         {
            if ( log_file_.is_open() )
            {
               log_file_.close();
            }
         }
      }
   }
}

//-----------------------------------------------------------------------------
void Logger::openLogFile(
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

   log_file_.open( log_path.string().c_str(), std::ofstream::out );
}

Logger liber::log::glog;

//-----------------------------------------------------------------------------
void liber::log::setPath(const std::string& path)
{
   liber::log::glog.setPath( path );
}

//-----------------------------------------------------------------------------
void liber::log::setSuffix(const std::string& suffix)
{
   liber::log::glog.setSuffix( suffix );
}

//-----------------------------------------------------------------------------
void liber::log::enable()
{
   liber::log::glog.setLogFileEnabled(true);
}

//-----------------------------------------------------------------------------
void liber::log::disable()
{
   liber::log::glog.setLogFileEnabled(false);
}

//-----------------------------------------------------------------------------
void liber::log::options(ui32 opts)
{
   liber::log::glog.setConsoleDisplayOptions(opts);
}

//-----------------------------------------------------------------------------
void liber::log::level(LogLevel level)
{
   liber::log::glog.setFilterLevel(level);
}

//-----------------------------------------------------------------------------
void liber::log::flush()
{
   liber::log::glog.flush();
}

//-----------------------------------------------------------------------------
void liber::log::print(LogLevel level, const char* format, va_list args)
{
   static char message_buffer[ MAX_MESSAGE_LEN_BYTES ];

   if ( format )
   {
      vsnprintf( message_buffer, sizeof( message_buffer ), format, args );
      liber::log::glog.send( LogMessage( level, std::string( message_buffer ) ) );
   }
}

//-----------------------------------------------------------------------------
void liber::log::raw( const char* format, ... )
{
   if (format)
   {
      va_list args;
      va_start( args, format );
      liber::log::print( Raw, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void liber::log::status(const char* format, ...)
{
   if (format)
   {
      va_list args;
      va_start( args, format );
      liber::log::print( Status, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void liber::log::error(const char* format, ...)
{
   if ( format )
   {
      va_list args;
      va_start( args, format );
      liber::log::print( Error, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void liber::log::warn(const char* format, ...)
{
   if ( format )
   {
      va_list args;
      va_start( args, format );
      liber::log::print( Warn, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void liber::log::debug( const char* format, ... )
{
   if ( format )
   {
      va_list args;
      va_start( args, format );
      liber::log::print( Debug, format, args );
      va_end( args );
   }
}

//-----------------------------------------------------------------------------
void liber::log::mem_dump(
   const char* header,
   const char* data_ptr,
   ui32        length,
   ui32        max_line_length )
{
   if ( ( header != NULL ) && ( data_ptr != NULL ) )
   {
      try
      {
         liber::log::glog.send( LogMessage(
            MemDump,
            header,
            data_ptr,
            length,
            max_line_length
         ));
      }
      catch ( ... )
      {
         liber::log::error(
            "liber::log::mem_dump - exception while logging dump\n" );
      }
   }
}


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



#ifndef  LOG_H
#define  LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <fstream>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "Queue.h"
#include "BinarySem.h"
#include "IThread.h"
#include "Timestamp.h"
#include "BaseTypes.h"
// #include "CountingSem.h"

namespace liber  {
namespace log    {

   // Console field configuration options:
   // Prepend the log message with the log level.
   static const ui32 DisplayLogLevel  = 0x00000001;
   // Prepend the log message with the message timestamp.
   static const ui32 DisplayTimestamp = 0x00000002;
   // Display all fields when printing the log message.
   static const ui32 DisplayAll       = 0xFFFFFFFF;

   // Log message severity enumeration. The log level is additionally used to
   // filter messages printed to the console. Only messages with a LogLevel
   // lower than the Logger's log level are printed to the console.  All
   // messages, regardless of level, are written to the log file.
   enum LogLevel {
      Supress = 0,
      Raw,
      Status,
      Error,
      Warn,
      Debug,
      MemDump,
      Verbose,
      InvalidLogLevel
   };


   // Convert the LogLevel enumeration to a string representation.
   const char* log_level_to_string( LogLevel level );


   ///
   /// Each log message has an associated severity, user-supplied log
   /// message, and finally a timestamp.  Log messages are queued
   /// by the logger and printed to a log file and the console.
   ///
   class LogMessage {
   public:

      ///
      /// Default constructor
      ///
      LogMessage();

      ///
      /// Construct a message with a specified severity and message.
      ///
      /// @param  level   Log message severity
      /// @param  message Log message text
      ///
      LogMessage( LogLevel level, const std::string& message );

      ///
      /// Construct a memory dump message. A memory dump message prints each
      /// byte in the buffer in hex format.
      ///
      /// @param  level    Log message severity
      /// @param  header   Log message header text (null-terminated string)
      /// @param  data_ptr Data buffer to dump
      /// @param  length   Length of data buffer in bytes
      /// @param  max_line_length  Maximum line length in characters
      ///
      LogMessage( LogLevel    level,
                  const char* header,
                  const char* data_ptr,
                  ui32        length,
                  ui32        max_line_length );

      ///
      /// Build the full message string.
      ///
      /// @param  format  Message display format bitmap
      ///
      std::string toString( ui32 format = log::DisplayAll ) const;

      ///
      ///
      ///
      void signalFlushed();

      bool waitFlush( i32 timeout_ms );

      LogLevel    log_level_;

      std::string message_;

      Timestamp   timestamp_;

      bool is_flush_marker_;

      BinarySem flush_signal_;
   };

   typedef  boost::shared_ptr<LogMessage> LogMessagePtr;


   class Logger : public liber::concurrency::IThread {
   public:

      ///
      /// Construct the Logger and launch the logging thread. By default,
      /// the logger does not send messages to a log file. Additionally, messages
      /// sent to the console are only prepended with the severity.
      ///
      Logger();

      ///
      /// Shutdown the logging thread and close the opened log file.
      /// Note: The logger makes no guarrantee that all messages in the queue
      /// will logged since the thread is cancelled.
      ///
      ~Logger();

      ///
      /// Enable/disable logging of messages to file.
      ///
      /// @param  enable  Enable/disable log message logging
      ///
      void setLogFileEnabled( bool enable );

      ///
      /// Sets the logging path. Defaults to the same directory as the
      /// executable.
      ///
      /// @param  path  Path to log directory
      ///
      void setPath( const std::string& path );

      ///
      /// Log file name suffix string. The log file name format is as follows:
      /// yyyy_mm_dd_HH_MM_ss_<suffix>_log.txt
      ///
      /// @param  suffix  Log file name suffix string
      ///
      void setSuffix( const std::string& suffix );

      ///
      /// Set the threshold severity level for log messages written to the
      /// console. Messages with a severity value =< the threshold are visible.
      /// Messages with a severity value > the threshold are suppressed.
      ///
      /// Messages are not filtered when written to a log file.
      ///
      /// @param  level  Threshold log level
      ///
      void setFilterLevel(LogLevel level);

      ///
      /// Send a new message to the logging queue.
      ///
      /// @param  message  Add a log message to the queue
      ///
      void send( LogMessagePtr message );

      ///
      /// Configure the display format for messages written to the console.
      /// Note: When a message is written to the log file, all fields are written.
      ///
      /// @param  display_format  Display format bitmap
      ///
      void setConsoleDisplayOptions( ui32 display_format );

   private:

      ///
      /// Thread loop
      ///
      void run( const bool& shutdown );

      ///
      // Open a new log file in the directory specified by "path". The "suffix"
      /// can be used to make the log names more specific.
      ///
      std::string generateLogName( const std::string& path, const std::string& suffix );

   private:

      // Display option bitmap
      ui32 console_log_options_;

      // Console display severity threshold
      LogLevel log_level_;

      // Log message message queue
      Queue<LogMessagePtr> messages_;

      // Mutex protecting access to log file suffix and root directory
      boost::mutex  file_attribute_lock_;

      // Log enable/disable flag
      bool          log_file_enabled_;

      // Log directory root path
      std::string   path_;

      // Log file suffix
      std::string   suffix_;

      // 
      ui32 current_log_file_size_;
   };

   // Global Logger instance
   extern Logger glog;

   ///
   /// Configure the log directory. By default, the directory is the current
   /// working directory.
   ///
   void set_path( const std::string& path );

   ///
   /// Configure the log file suffix.
   ///
   void set_suffix(const std::string& suffix);

   ///
   /// Enable the Logger to write messages to a log file.
   ///
   void enable();

   ///
   /// Disable the Logger from writing messages to a log file.
   ///
   void disable();

   ///
   /// Configure how the Logger will print messages to console.
   ///
   void options(ui32 opts);

   ///
   /// Configure the Logger severity filter.
   ///
   void level(LogLevel level);

   ///
   /// Wait for all messages preceding the call to flush to be written to the
   /// console.
   ///
   void flush( i32 timeout_ms = liber::thread::Semaphore::SemWaitForever );

   void print(LogLevel level, const char* format, va_list arg);
   void raw(const char* format, ...);
   void status(const char* format, ...);
   void error(const char* format, ...);
   void warn(const char* format, ...);
   void debug(const char* format, ...);
   void mem_dump(const char* header, const char* pData, ui32 length, ui32 max_line_length = 16);

} // End of namespace log
} // End of namespace liber

#endif // LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <fstream>
#include <boost/thread/mutex.hpp>
#include "Queue.h"
#include "IThread.h"
#include "Timestamp.h"
#include "BaseTypes.h"
#include "CountingSem.h"

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
    Verbose
  };


  // Convert the LogLevel enumeration to a string representation.
  inline std::string LogLevelToString(LogLevel level)
  {
    std::string levelString = "Unknown";
    switch (level)
    {
      case Raw: levelString = ""; break;
      case Status: levelString = "Status"; break;
      case Error:  levelString = "Error";  break;
      case Warn:   levelString = "Warn";   break;
      case Debug:  levelString = "Debug";  break;
      case MemDump:  levelString = "MemDump";  break;
      case Verbose:  levelString = "Verbose";  break;
      default: break;
    }
    return levelString;
  };


  // Each log message has an associated severity, user-supplied log
  // message, and finally a timestamp.  Log messages are queued
  // by the logger and printed to a log file and the console.
  class LogMessage {
  public:

    LogMessage() {};

    /**
     * Construct a message with a specified severity and message.
     * The constructor automatically timestamps the message.
     */
    LogMessage(LogLevel level, const std::string& message);

    /**
     *
     */
    LogMessage(LogLevel level,
               const char* header,
               const char* pData, ui32 nBytes,
               ui32 nBytesPerRow);

    /**
     * Convert the message to a string. The message formate can be configured.
     */
    std::string toString(ui32 format = log::DisplayAll) const;

    LogLevel    mLevel;
    std::string mMessage;
    Timestamp   mTimestamp;
  };


  class Logger : public liber::concurrency::IThread {
  public:

    /**
     * Construct the Logger and launch the logging thread. By default,
     * the logger does not send messages to a log file. Additionally, messages
     * sent to the console are only prepended with the severity.
     */
    Logger();

    /**
     * Shutdown the logging thread and close the opened log file.
     * Note: The logger makes no guarrantee that all messages in the queue
     * will logged since the thread is cancelled.
     */
    ~Logger();

    /**
     * Sets the logging path. Defaults to the same directory
     * as the executable.
     */
    void setLogFileEnabled(bool bEnable);
    void setPath(const std::string& path);
    void setSuffix(const std::string& suffix);

    void setFilterLevel(LogLevel level);

    /**
     * Send a new message to the logging queue.
     */
    void send(const LogMessage& message);

    /**
     * Configure the display format for messages written to the console.
     * Note: When a message is written to the log file, all fields are written.
     */
    void setConsoleDisplayOptions(ui32 displayOptions);

    void flush();

  private:

    void run(const bool& bShutdown);

    /**
     * Open a new log file in the directory specified by "path". The "suffix"
     * can be used to make the log names more specific.
     */
    void openLogFile(const std::string& path, const std::string& suffix);

  private:

    ui32 mConsoleLogOpts;

    LogLevel mLevel;
    Queue<LogMessage> mMessages;

    bool mbAllowMessages;

    boost::mutex file_attribute_lock_;
    bool mbLogFileEnabled;
    std::string   mPath;
    std::string   mSuffix;
    std::ofstream mFile;
    ui32 mnFileSizeBytes;
  };

  // Global Logger instance
  extern Logger glog;

  /**
   * Configure the log directory. By default, the directory is the current
   * working directory.
   */
  void setPath(const std::string& path);

  /**
   * Configure the log file suffix.
   */
  void setSuffix(const std::string& suffix);

  /**
   * Enable the Logger to write messages to a log file.
   */
  void enable();

  /**
   * Disable the Logger from writing messages to a log file.
   */
  void disable();

  /**
   * Configure how the Logger will print messages to console.
   */
  void options(ui32 opts);

  /**
   * Configure the Logger severity filter.
   */
  void level(LogLevel level);

  void flush();

  void print(LogLevel level, const char* format, va_list arg);
  void raw(const char* format, ...);
  void status(const char* format, ...);
  void error(const char* format, ...);
  void warn(const char* format, ...);
  void debug(const char* format, ...);
  void mem_dump(const char* header, const char* pData, ui32 nBytes, ui32 nBytesPerRow = 16);

} // End of namespace log
} // End of namespace liber


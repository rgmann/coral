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
: mLevel(level)
, mMessage(message)
, mTimestamp(Timestamp::Now())
{
}

//-----------------------------------------------------------------------------
LogMessage::LogMessage(LogLevel level,
                       const char* header,
                       const char* pData, ui32 nBytes,
                       ui32 nBytesPerRow)
: mLevel(level)
, mTimestamp(Timestamp::Now())
{
  char rowPrefix[7];
  std::stringstream stream;

  if (strlen(header) > 0)
  {
    stream << header << "\n";
  }

  ui32 lnOffset = 0;
  for (; lnOffset < nBytes; lnOffset++)
  {
    if (lnOffset % nBytesPerRow == 0)
    {
      snprintf(rowPrefix, sizeof(rowPrefix), "%4X: ", lnOffset);
      stream << rowPrefix;
    }

    snprintf(rowPrefix, sizeof(rowPrefix), "%02X ", (unsigned char)pData[lnOffset]);
    stream << rowPrefix;
    if (lnOffset > 0 && ((lnOffset+1) % nBytesPerRow == 0))
    {
      stream << "\n";
    }
  }
  if (lnOffset > 0) stream << "\n";

  mMessage.assign(stream.str());
}

//-----------------------------------------------------------------------------
std::string LogMessage::toString(ui32 format) const
{
  std::stringstream ss;

  if (format & log::DisplayTimestamp)
  {
    ss << std::fixed << mTimestamp.fseconds() << std::dec << " ";
  }

  if (format & log::DisplayLogLevel)
  {
    ss << LogLevelToString(mLevel) ;
    if ( mLevel != Raw ) ss << ":";
    ss << " ";
  }

  ss << mMessage;

  return ss.str();
}


//-----------------------------------------------------------------------------
Logger::Logger()
: liber::concurrency::IThread("CommonLoggingThread")
, mConsoleLogOpts(log::DisplayLogLevel)
, mLevel(liber::log::Status)
, mbAllowMessages(true)
, mbLogFileEnabled(false)
, mPath(boost::filesystem::current_path().generic_string())
, mSuffix("")
, mnFileSizeBytes(0)
{
  launch();
}

//-----------------------------------------------------------------------------
Logger::~Logger()
{
  cancel(true);
  mFile.close();
}

//-----------------------------------------------------------------------------
void Logger::setLogFileEnabled(bool bEnable)
{
  mbLogFileEnabled = bEnable;
}

//-----------------------------------------------------------------------------
void Logger::setPath(const std::string& path)
{
  boost::mutex::scoped_lock guard( file_attribute_lock_ );
  mPath = path;
}

//-----------------------------------------------------------------------------
void Logger::setSuffix(const std::string& suffix)
{
  boost::mutex::scoped_lock guard( file_attribute_lock_ );
  mSuffix = suffix;
}

//-----------------------------------------------------------------------------
void Logger::setFilterLevel(LogLevel level)
{
  mLevel = level;
}

//-----------------------------------------------------------------------------
void Logger::setConsoleDisplayOptions(ui32 displayOptions)
{
  mConsoleLogOpts = displayOptions;
}

//-----------------------------------------------------------------------------
void Logger::send(const LogMessage& message)
{
  if (mbAllowMessages)
  {
    mMessages.push(message, 0);
  }
}

//-----------------------------------------------------------------------------
void Logger::flush()
{
  mbAllowMessages = false;
  while (mMessages.size() > 0)
  {
    usleep(100000);
  }
  mbAllowMessages = true;
}

//-----------------------------------------------------------------------------
void Logger::run(const bool& bShutdown)
{
  LogMessage lMessage;
  std::string lPath = mPath;
  std::string lSuffix = mSuffix;

  while (!bShutdown)
  {
    if (mMessages.pop(lMessage))
    {
      if (lMessage.mLevel <= mLevel)
      {
        std::cout << lMessage.toString(mConsoleLogOpts);
      }

      if (mbLogFileEnabled)
      {
        {
          boost::mutex::scoped_lock guard( file_attribute_lock_ );
          lPath = mPath;
          lSuffix = mSuffix;
        }

        if (!mFile.is_open())
        {
          mFile.close();
          openLogFile(lPath, lSuffix);
        }

        if (mFile.is_open())
        {
          std::string lLogStr = lMessage.toString(log::DisplayAll);
          mnFileSizeBytes += lLogStr.size();
          mFile << lLogStr;
        }
      }
      else
      {
        if (mFile.is_open())
        {
          mFile.close();
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
void Logger::
openLogFile(const std::string& path, const std::string& suffix)
{
  Date lDate;

  lDate.sample();
  std::stringstream ss;
  ss << lDate.year() << "_";
  ss << (lDate.month() + 1) << "_";
  ss << lDate.dayOfMonth() << "_";
  ss << lDate.hour() << "_";
  ss << lDate.minute() << "_";
  ss << lDate.second() << "_";

  if (!suffix.empty())
  {
    ss << suffix << "_";
  }
  ss << "log.txt";

  mnFileSizeBytes = 0;

  boost::filesystem::path lPath(path);
  lPath /= ss.str();

  mFile.open( lPath.string().c_str(), std::ofstream::out );
}

Logger liber::log::glog;

//-----------------------------------------------------------------------------
void liber::log::setPath(const std::string& path)
{
  liber::log::glog.setPath(path);
}

//-----------------------------------------------------------------------------
void liber::log::setSuffix(const std::string& suffix)
{
  liber::log::glog.setSuffix(suffix);
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
  static char message_buffer[MAX_MESSAGE_LEN_BYTES];
  if (format)
  {
    vsnprintf( message_buffer, sizeof( message_buffer ), format, args );
    liber::log::glog.send( LogMessage( level, std::string( message_buffer ) ) );
  }
}

//-----------------------------------------------------------------------------
void liber::log::raw(const char* format, ...)
{
  if (format)
  {
    va_list args;
    va_start(args, format);
    liber::log::print(Raw, format, args);
    va_end(args);
  }
}

//-----------------------------------------------------------------------------
void liber::log::status(const char* format, ...)
{
  if (format)
  {
    va_list args;
    va_start(args, format);
    liber::log::print(Status, format, args);
    va_end(args);
  }
}

//-----------------------------------------------------------------------------
void liber::log::error(const char* format, ...)
{
  if (format)
  {
    va_list args;
    va_start(args, format);
    liber::log::print(Error, format, args);
    va_end(args);
  }
}

//-----------------------------------------------------------------------------
void liber::log::warn(const char* format, ...)
{
  if (format)
  {
    va_list args;
    va_start(args, format);
    liber::log::print(Warn, format, args);
    va_end(args);
  }
}

//-----------------------------------------------------------------------------
void liber::log::debug(const char* format, ...)
{
  if (format)
  {
    va_list args;
    va_start(args, format);
    liber::log::print(Debug, format, args);
    va_end(args);
  }
}

//-----------------------------------------------------------------------------
void liber::log::mem_dump(const char* header, const char* pData, ui32 nBytes, ui32 nBytesPerRow)
{
  if ((header != NULL) && (pData != NULL))
  {
    try
    {
      liber::log::glog.send(LogMessage(MemDump, header, pData, nBytes, nBytesPerRow));
    }
    catch (...)
    {
      liber::log::error("liber::log::mem_dump - exception while logging dump\n");
    }
  }
}


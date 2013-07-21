#ifndef LOG_H
#define LOG_H

#include <map>
#include <string>
#include <fstream>
#include "Queue.h"
#include "IThread.h"

class Log
{
public:
   
   enum LogLevel {
      Unset       = 0,
      Error,
      Warning,
      Info,
      Trace,
      NumLogLevels
   };
   
   class LogLine
   {
   public:
      
      LogLine();
      
      LogLine(int);
      
      LogLine(unsigned int);
      
      LogLine(char);
      
      LogLine(const std::string&);
      
      LogLine(const char*);
      
      LogLine(const IThread&);
      
      LogLine(LogLevel);
      
      friend LogLine& operator << (LogLine &left, const LogLine &right);
      
      std::string getContextStr() const;
      
      std::string mLine;
      
      LogLevel mLevel;
      
      std::string mContextName;
      int         mnContextId;
   };
   
   static bool Setup(const std::string &appname,
                     const std::string &root,
                     bool bSeparate = true);
   
   static bool Teardown();
   
   static bool FlushTrace();
   
   static void PrintToConsole(bool bEnable);
   
   static void SetLogSizeLimit(ui32 nMaxSize);
   
   static void log(const LogLine &line, LogLevel level = Unset);
   
   static std::string LevelToString(LogLevel level);
   
private:
   
   Log(LogLevel level = Info);
   
   ~Log();
   
   bool open();
   
   bool close();
   
   bool createNewFile(LogLevel level);
   
private:
   
   static Log ourInstance;
      
   bool mbIsSetup;
   
   bool mbPrintToConsole;
   
   bool mbSeperateLevels;
   
   std::string mAppName;
   
   std::string mBaseName;
      
   Queue<std::string> mTraceQueue;
   
   std::ofstream mStream[NumLogLevels];
   
   ui32 mynSizeLimitBytes;
};

#define ERROR(line) \
{ \
   Log::LogLine lLine; \
   Log::log(lLine << line, Log::Error); \
}

#define WARN(line) \
{ \
   Log::LogLine lLine; \
   Log::log(lLine << line, Log::Warning); \
}

#define INFO(line) \
{ \
   Log::LogLine lLine; \
   Log::log(lLine << line, Log::Info); \
}

#define TRACE(line) \
{ \
   Log::LogLine lLine; \
   Log::log(lLine << line, Log::Trace); \
}

#define LOG(line) \
{ \
   Log::LogLine lLine; \
   Log::log(lLine << line); \
}

#endif // LOG_H
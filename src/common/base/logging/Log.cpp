#include <iostream>
#include <sstream>
#include "Log.h"
#include "Date.h"

#define LOG_EXT   (const char *)"llog"

Log Log::ourInstance;

//------------------------------------------------------------------------------
Log::LogLine::LogLine()
: mLine(std::string(""))
, mLevel(Unset)
, mContextName("main")
, mnContextId(0)
{
}

//------------------------------------------------------------------------------
Log::LogLine::LogLine(int val)
: mLevel(Unset)
, mContextName("main")
, mnContextId(0)
{
   std::stringstream ss;
   ss << val;
   mLine = ss.str();
}

//------------------------------------------------------------------------------
Log::LogLine::LogLine(unsigned int val)
: mLevel(Unset)
, mContextName("main")
, mnContextId(0)
{
   std::stringstream ss;
   ss << val;
   mLine = ss.str();
}

//------------------------------------------------------------------------------
Log::LogLine::LogLine(char val)
: mLine("")
, mLevel(Unset)
, mContextName("main")
, mnContextId(0)
{
   mLine += val;
}

//------------------------------------------------------------------------------
Log::LogLine::LogLine(const std::string &val)
: mLine(std::string(val))
, mLevel(Unset)
, mContextName("main")
, mnContextId(0)
{
}

//------------------------------------------------------------------------------
Log::LogLine::LogLine(const char* val)
: mLine(std::string(val))
, mLevel(Unset)
, mContextName("main")
, mnContextId(0)
{
}

//------------------------------------------------------------------------------
Log::LogLine::LogLine(const IThread& val)
: mLine(std::string(""))
, mLevel(Unset)
, mnContextId(0)
{
   mContextName = val.getName();
   mnContextId = val.getId();
}

//------------------------------------------------------------------------------
Log::LogLine::LogLine(LogLevel level)
: mLine(std::string(""))
, mLevel(level)
, mContextName("main")
, mnContextId(0)
{
}

//------------------------------------------------------------------------------
Log::LogLine& operator << (Log::LogLine &left, const Log::LogLine &right)
{
   left.mLine += right.mLine;
   
   if (left.mLevel == Log::Unset)
   {
      left.mLevel = right.mLevel;
   }
   
   return left;
}

//------------------------------------------------------------------------------
std::string Log::LogLine::getContextStr() const
{
   std::stringstream ss;
   
   ss << mContextName;
   ss << "(";
   ss << mnContextId;
   ss << ")";
   
   return ss.str();
}

//------------------------------------------------------------------------------
Log::Log(LogLevel level)
: mbIsSetup(false)
, mbPrintToConsole(false)
, mbSeperateLevels(false)
{
}

//------------------------------------------------------------------------------
Log::~Log()
{
}

//------------------------------------------------------------------------------
bool Log::Setup(const std::string &appname,
                const std::string &root,
                bool bSeparate)
{
   bool lbSuccess = true;
   ourInstance.mBaseName = root + "/" + appname;
   
   if (ourInstance.mbIsSetup) return false;
   
   if (bSeparate)
   {
      ourInstance.mbSeperateLevels = true;
      std::string lFilename = ourInstance.mBaseName + 
                              "_error." + std::string(LOG_EXT);
      ourInstance.mStream[Error].open(lFilename.c_str());
      lbSuccess &= ourInstance.mStream[Error].is_open();
   
      lFilename = ourInstance.mBaseName + 
                  "_warning." + std::string(LOG_EXT);
      ourInstance.mStream[Warning].open(lFilename.c_str());
      lbSuccess &= ourInstance.mStream[Warning].is_open();
   
      lFilename = ourInstance.mBaseName + "_info." + std::string(LOG_EXT);
      ourInstance.mStream[Info].open(lFilename.c_str());
      lbSuccess &= ourInstance.mStream[Info].is_open();
   
      lFilename = ourInstance.mBaseName + "_trace." + std::string(LOG_EXT);
      ourInstance.mStream[Trace].open(lFilename.c_str());
      lbSuccess &= ourInstance.mStream[Trace].is_open();
   }
   else
   {
      std::string lFilename = ourInstance.mBaseName + 
                              "_all." + std::string(LOG_EXT);
      ourInstance.mStream[0].open(lFilename.c_str());
      lbSuccess &= ourInstance.mStream[0].is_open();
   }
   
   ourInstance.mAppName = appname;
   ourInstance.mbIsSetup = true;
   ourInstance.mTraceQueue.initialize();
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool Log::Teardown()
{
   if (!ourInstance.mbIsSetup) return false;
   
   // Flush the queue.
   FlushTrace();
   
   for (int lLevel = 0; lLevel < NumLogLevels; lLevel++)
   {
      ourInstance.mStream[lLevel].close();
   }
   
   ourInstance.mTraceQueue.initialize();
   
   ourInstance.mbIsSetup = false;
   
   return true;
}

//------------------------------------------------------------------------------
bool Log::FlushTrace()
{
   std::string lTraceStr;
   
   if (!ourInstance.mbIsSetup) return false;
   
   while (ourInstance.mTraceQueue.pop(lTraceStr, 10))
   {
      int lnStreamInd = ourInstance.mbSeperateLevels ? Trace : 0;
      
      ourInstance.mStream[lnStreamInd] << lTraceStr;
   }
   
   return true;
}

//------------------------------------------------------------------------------
void Log::PrintToConsole(bool bEnable)
{
   ourInstance.mbPrintToConsole = bEnable;
}

//------------------------------------------------------------------------------
void Log::SetLogSizeLimit(ui32 nMaxSize)
{
   mynSizeLimitBytes = nMaxSize;
}

//------------------------------------------------------------------------------
std::string Log::LevelToString(LogLevel level)
{
   std::string lLevelStr = "";
   switch (level) {
      case Error:
         lLevelStr = "ERROR";
         break;
      case Warning:
         lLevelStr = "WARN";
         break;
      case Info:
         lLevelStr = "INFO";
         break;
      case Trace:
         lLevelStr = "TRACE";
         break;
      default:
         lLevelStr = "UNSET";
         break;
   }
   return lLevelStr;
}

//------------------------------------------------------------------------------
std::string Log::LevelToLogSuffix(LogLevel level)
{
   std::string lLevelStr = "";
   switch (level) {
      case Error:
         lLevelStr = "_error";
         break;
      case Warning:
         lLevelStr = "_warn";
         break;
      case Info:
         lLevelStr = "_info";
         break;
      case Trace:
         lLevelStr = "_trace";
         break;
      default:
         lLevelStr = "_all";
         break;
   }
   return lLevelStr;
}

//------------------------------------------------------------------------------
void Log::log(const LogLine &line, LogLevel level)
{
   LogLevel lLevel = level;
   Date lDate;
   std::stringstream lLogSs;
   
   if (level == Unset)
   {
      lLevel = (line.mLevel != Unset) ? line.mLevel : Info;
   }
   
   lLogSs   << "[" << lDate.getString(Date::LOCAL)
            << "] "
            << LevelToString(lLevel)
            << " {"
            << ourInstance.mAppName << "."
            << line.getContextStr()
            << "} "
            << line.mLine
            << std::endl;
   
   if (ourInstance.mbPrintToConsole)
   {
      std::cout << lLogSs.str();
   }
   
   if (!ourInstance.mbIsSetup) return;
   
   int lnStreamInd = ourInstance.mbSeperateLevels ? lLevel : 0;
   if (lLevel != Trace)
   {
      if (mnCurrentFileSize >= mynSizeLimitBytes)
      {
         createNewFile(lnStreamInd);
      }
      
      ourInstance.mTraceQueue.push(lLogSs.str(), 0);
   }
   else
   {
      ourInstance.mStream[lnStreamInd] << lLogSs.str();
   }
}

//------------------------------------------------------------------------------
bool Log::createNewFile(int nStreamInd)
{
   Date lDate;
   std::stringstream lNameSs;
   
   ourInstance.mStream[nStreamInd].close();
   
   lNameSs  << ourInstance.mBaseName
            << LevelToLogSuffix(nStreamInd)
            << lDate.getString(Date::LOCAL, "MMDDYYYY_S")
            << ".llog";
   
   ourInstance.mStream[nStreamInd].open(lNameSs.str());
   
   return (ourInstance.mStream[nStreamInd].is_open());
}

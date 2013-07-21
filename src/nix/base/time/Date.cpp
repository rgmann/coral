#include <sys/time.h>
#include <sstream>
#include "Date.h"

//------------------------------------------------------------------------------
Date::Date()
{
   sample();
}

//------------------------------------------------------------------------------
Date::Date(time_t secs)
{
   myRawTime.seconds(secs);
}

//------------------------------------------------------------------------------
time_t Date::sample()
{
//   time(&myRawTime);
   myRawTime.sample();
   gmtime_r(myRawTime.ptr(), &myUtcTime);
   localtime_r(myRawTime.ptr(), &myLocalTime);
   return myRawTime.seconds();
}

//------------------------------------------------------------------------------
ui64 Date::sts(Zone z)
{
   struct tm lTm;
   
   memset(&lTm, 0, sizeof(lTm));
   lTm.tm_mday = 1;
   lTm.tm_year = 70; // 1970
   
   return (ui64)difftime(myRawTime.seconds(), mktime(&lTm));
}

//------------------------------------------------------------------------------
ui32 Date::year(Zone z)
{
   return getZone(z)->tm_year + 1900;
}

//------------------------------------------------------------------------------
ui32 Date::month(Zone z)
{
   return getZone(z)->tm_mon;
}

//------------------------------------------------------------------------------
ui32 Date::dayOfWeek(Zone z)
{
   return getZone(z)->tm_wday;
}

//------------------------------------------------------------------------------
ui32 Date::dayOfMonth(Zone z)
{
   return getZone(z)->tm_mday;
}

//------------------------------------------------------------------------------
ui32 Date::dayOfYear(Zone z)
{
   return getZone(z)->tm_yday;
}

//------------------------------------------------------------------------------
ui32 Date::hour(Zone z)
{
   return getZone(z)->tm_hour % 12;
}

//------------------------------------------------------------------------------
ui32 Date::astroHour(Zone z)
{
   return getZone(z)->tm_hour;
}

//------------------------------------------------------------------------------
ui32 Date::minute()
{
   return myUtcTime.tm_min;
}

//------------------------------------------------------------------------------
ui32 Date::second()
{
   return myUtcTime.tm_sec;
}

//------------------------------------------------------------------------------
Timestamp Date::timestamp()
{
   return myRawTime;
}

//------------------------------------------------------------------------------
const char* Date::Month(ui32 month)
{
   static const char* MonthStrings[] = {  "January", "February", "March",
                                          "April", "May", "June", "July",
                                          "August", "September", "October",
                                          "November", "December"           };
   
   if (month <= MaxMonth)
      return MonthStrings[month];
   else
      return "\0";
}

//------------------------------------------------------------------------------
const char* Date::Day(ui32 day)
{
   static const char* DayStrings[] = { "Saturday", "Monday", "Tuesday",
                                       "Wednesday", "Thursday", "Friday",
                                       "Sunday"};
   
   if (day <= DaysPerWeek)
      return DayStrings[day];
   else
      return "\0";
}

//------------------------------------------------------------------------------
struct tm* Date::getZone(Zone z)
{
   if (z == UTC)
      return &myUtcTime;
   else if (z == LOCAL)
      return &myLocalTime;
   
   return &myUtcTime;
}

//------------------------------------------------------------------------------
std::string Date::getString(Zone z, const std::string &fmt)
{
   std::stringstream ss;
      
   ss << month(z) << "/"
      << dayOfMonth(z) << "/"
      << year(z) << " "
      << astroHour(z) << ":"
      << minute() << ":"
      << (((f32)second()) + myRawTime.fseconds());
   
   return ss.str();
}
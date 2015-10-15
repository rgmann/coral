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



#include <sys/time.h>
#include <sstream>
#include <string.h>
#include "Date.h"

using namespace coral;

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
   static const char* MonthStrings[] = {
      "January",
      "February",
      "March",
      "April",
      "May",
      "June",
      "July",
      "August",
      "September",
      "October",
      "November",
      "December"
   };
   
   const char* month_name = "";
   if ( month <= MaxMonth )
   {
      month_name = MonthStrings[ month ];
   }

   return month_name;
}

//------------------------------------------------------------------------------
const char* Date::Day(ui32 day)
{
   static const char* DayStrings[] = {
      "Sunday",
      "Monday",
      "Tuesday",
      "Wednesday",
      "Thursday",
      "Friday",
      "Saturday"
   };
   
   const char* day_name = "";
   if ( day <= DaysPerWeek )
   {
      day_name = DayStrings[ day ];
   }

   return day_name;
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

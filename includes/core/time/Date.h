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



#ifndef DATE_H
#define DATE_H

#include <time.h>
#include <string>
#include "BaseTypes.h"
#include "Timestamp.h"

namespace coral {

class Date {
public:
   
   Date();
         
   explicit Date(time_t secs);
      
   time_t sample();
   
   enum Zone {
      UTC,
      LOCAL
   };

   /**
    * Time in secs since epoch.
    */
   ui64  sts(Zone z = UTC);
   
   ui32  year(Zone z = UTC);
   
   static const ui32 MinMonth = 0;
   static const ui32 MaxMonth = 11;
   static const ui32 DaysPerWeek = 7;
   ui32  month(Zone z = UTC);
   
   ui32  dayOfWeek(Zone z = UTC);
   
   ui32  dayOfMonth(Zone z = UTC);

   ui32  dayOfYear(Zone z = UTC);
   
   ui32  hour(Zone z = UTC);
      
   ui32  astroHour(Zone z = UTC);
   
   ui32  minute();
   
   ui32  second();
   
   Timestamp timestamp();
   
   std::string toString();
   
   Date& operator = (const Date &other);
   
   static const char* Month(ui32 month);
   static const char* Day(ui32 day);
   
   std::string getString(Zone z = UTC, const std::string &fmt = "");
   
private:
   
   struct tm* getZone(Zone z);
   
private:
   
   Timestamp   myRawTime;
   struct tm   myUtcTime;
   struct tm   myLocalTime;
};

} // namespace coral
#endif // DATE_H
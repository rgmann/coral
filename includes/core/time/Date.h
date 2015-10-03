#ifndef DATE_H
#define DATE_H

#include <time.h>
#include <string>
#include "BaseTypes.h"
#include "Timestamp.h"

namespace liber {

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

} // namespace liber
#endif // DATE_H
#ifndef TIME_H
#define TIME_H

#include <sys/time.h>
#include "BaseTypes.h"

class Timestamp
{
public:
   
   Timestamp();
   
   ~Timestamp();

   static Timestamp Now();
   
   void  sample();
   
   time_t seconds();
   
   void seconds(time_t seconds);
   
   time_t* ptr();
   
   f32   fseconds();
   
   ui64   microseconds();
   
   i64  diffInMs(const Timestamp &start);
   
   i64  diffInS(const Timestamp &start);
   
   i64  diffInUs(const Timestamp &start);
   
private:
   
   struct timeval mTV;
};

#endif // TIME_H

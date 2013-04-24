#ifndef TIME_H
#define TIME_H

#include <sys/time.h>
#include "BaseTypes.h"

class Timestamp
{
public:
   
   Timestamp();
   
   ~Timestamp();
   
   void  sample();
   
   i64  diffInMs(const Timestamp &start);
   
   i64  diffInS(const Timestamp &start);
   
   i64  diffInUs(const Timestamp &start);
   
private:
   
   struct timeval m_TV;
};

#endif // TIME_H
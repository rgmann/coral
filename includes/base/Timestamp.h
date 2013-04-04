#ifndef TIME_H
#define TIME_H

#include <sys/time.h>

class Timestamp
{
public:
   
   Timestamp();
   
   ~Timestamp();
   
   void  sample();
   
   long  diffInMs(const Timestamp &start);
   
   long  diffInS(const Timestamp &start);
   
   long  diffInUs(const Timestamp &start);
   
private:
   
   struct timeval m_TV;
};

#endif // TIME_H
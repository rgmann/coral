#include "Sem.h"
#include <pthread.h>

//------------------------------------------------------------------------------
Sem::~Sem()
{
}

//------------------------------------------------------------------------------
void  Sem::SetTimeoutTime(struct timespec *ts, int nTimeoutMs)
{
   struct timeval    tv;
   
   gettimeofday(&tv, NULL);
   ts->tv_sec = tv.tv_sec + (nTimeoutMs / 1000);
   ts->tv_nsec = tv.tv_usec * 1000 + 1000000 * (nTimeoutMs % 1000);
   
   if (ts->tv_nsec > 1000000000)
   {
      ts->tv_sec++;
      ts->tv_nsec = ts->tv_nsec % 1000000000;
   }
}

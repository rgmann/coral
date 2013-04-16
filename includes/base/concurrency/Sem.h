#ifndef SEM_H
#define SEM_H

#include <time.h>
#include <sys/time.h>

struct SemPriv;

class Sem
{
public:
   
   enum SemStatus
   {
      SemOk,
      SemAcquired,
      SemTimeout,
      SemError
   };
   
   enum SemTimeout
   {
      SemWaitForever = -1
   };
   
   virtual SemStatus take(int nTimeoutMs = SemWaitForever) = 0;
   
   virtual SemStatus give() = 0;
   
protected:
   
   static void SetTimeoutTime(struct timespec *ts, int nTimeoutMs);
   
protected:
   
   struct SemPriv* m_pSem;
};

#endif // SEM_H
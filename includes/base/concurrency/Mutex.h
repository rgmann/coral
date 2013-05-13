#ifndef MUTEX_H
#define MUTEX_H

#include "BaseTypes.h"

struct PlatformMutex;

class Mutex
{
public:
   
   static const i32 WaitForever = -1;
   
   Mutex();
   
   ~Mutex();
   
   bool  lock(i32 nTimeoutMs = WaitForever);
   
   bool  unlock();
   
private:
   
   struct PlatformMutex *m_pMutex;
};

#endif // MUTEX_H
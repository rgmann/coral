#ifndef MUTEX_H
#define MUTEX_H

#define WAIT_FOREVER    -1

struct PlatformMutex;

class Mutex
{
public:
   
   Mutex();
   
   ~Mutex();
   
   bool  lock(int nTimeoutMs = WAIT_FOREVER);
   
   bool  unlock();
   
private:
   
   struct PlatformMutex *m_pMutex;
};

#endif // MUTEX_H
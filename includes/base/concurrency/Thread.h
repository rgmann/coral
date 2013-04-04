#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
//#include "ThreadTypes.h"

class ThreadArg
{
public:
   
   ThreadArg() { m_bStopSignal = false; };
   
   bool  stopSignalled() const { return m_bStopSignal; };
   
   void* pUserData;
   
private:
   
   bool  m_bStopSignal;
   
   friend class Thread;
};

struct ThreadRet
{
   void* pUserData;
};

typedef void (*UserThreadFunc)(ThreadArg* pArg);

class Thread
{
public:
   
   ~Thread();
   
   static Thread* Create(UserThreadFunc func, void* pArg);
   
   bool  join(ThreadRet* pRet = NULL);
   
   //bool  exit();
   
   //bool  cancel();
   
   //bool  detach();
   
   bool  stop();
   
private:
   
   /**
    * Use PThread::Create
    */
   Thread();
   
   static void* ThreadFunc(void* pArg);
   
   static void CleanupHandler(void* pArg);
   
   bool  isActive();
   
private:
   
   struct ThreadPriv* m_pThread;
   
   bool           m_bStarted;
   
   bool           m_bDone;
   
   UserThreadFunc m_pUserFunc;
   
   ThreadArg*     m_pThreadArg;
};

#endif // THREAD_H
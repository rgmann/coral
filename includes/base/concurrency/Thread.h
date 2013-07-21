#ifndef THREAD_H
#define THREAD_H

#include <string>
#include "BaseTypes.h"
#include "Mutex.h"


class ThreadArg
{
public:
   
   ThreadArg() { m_bStopSignal = false; };
   
   bool  stopSignalled() const { return m_bStopSignal; };
   
   void* pUserData;
      
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
   
   bool  isActive() const;
   
   ui32 getId() const;
   
private:
   
   /**
    * Use PThread::Create
    */
   Thread();
   
   static void* ThreadFunc(void* pArg);
   
   static void CleanupHandler(void* pArg);
   
private:
   
   static Mutex   ourIdMutex;
   
   static ui32    ournCurrentTid;
   
   struct ThreadPriv* m_pThread;
   
   bool           m_bStarted;
   
   bool           m_bDone;
   
   UserThreadFunc m_pUserFunc;
   
   ThreadArg*     m_pThreadArg;
   
   ui32           m_nId;
};

#endif // THREAD_H
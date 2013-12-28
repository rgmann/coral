#ifndef RSYNC_THREAD_H
#define RSYNC_THREAD_H

#include "Queue.h"
#include "Thread.h"
#include "RsyncPacket.h"

class RsyncThread
{
public:
   
   RsyncThread(ui32 inQDepth = 10, ui32 outQDepth = 10);
   
   virtual ~RsyncThread();
   
   static bool Register(RsyncPacket::Type type, RsyncThread* pThread);
   static bool Unregister(RsyncPacket::Type type);
   
   static bool RoutePacket(RsyncPacket* pPacket);
   
   virtual bool start();
   
   virtual void stop();
   
   bool pushPacket(RsyncPacket* pPacket, ui32 timeoutMs = 100);
   
   bool popPacket(RsyncPacket* pPacket, ui32 timeoutMs = 100);
   
   void flushInput();
   
   void flushOutput();
   
protected:
   
   static void RsyncThreadFunc(ThreadArg* pArg);
   
   virtual void rsyncThreadFunc(ThreadArg* pArg);
   
private:
   
   static RsyncThread*  RegistrationTable[RsyncPacket::NumTypes];
   static Mutex         RegTableLock;
   
   Queue<RsyncPacket*> m_InQueue;
   Queue<RsyncPacket*> m_OutQueue;
   
   Thread*  m_pRsyncThread;
};

#endif // RSYNC_THREAD_H
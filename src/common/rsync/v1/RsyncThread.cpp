#include "RsyncThread.h"

RsyncThread*   RsyncThread::RegistrationTable[] = { NULL };

//------------------------------------------------------------------------------
RsyncThread::RsyncThread(ui32 inQDepth, ui32 outQDepth)
{
   m_InQueue.initialize(inQDepth);
   m_OutQueue.initialize(outQDepth);
}

//------------------------------------------------------------------------------
RsyncThread::~RsyncThread()
{
   stop();
}

//------------------------------------------------------------------------------
bool RsyncThread::Register(RsyncPacket::Type type, RsyncThread* pThread)
{
   bool l_bSuccess = true;
   
   RegTableLock.lock(Mutex::WaitForever);
   
   if (RegistrationTable[type]) l_bSuccess = false;
   
   RegistrationTable[type] = pThread;
   
   RegTableLock.unlock();
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool RsyncThread::Unregister(RsyncPacket::Type type)
{
   bool l_bSuccess = true;
   
   RegTableLock.lock(Mutex::WaitForever);
   
   if (RegistrationTable[type] == NULL) l_bSuccess = false;
   
   RegistrationTable[type] = NULL;
   
   RegTableLock.unlock();
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncThread::RoutePacket(RsyncPacket *pPacket)
{
   bool l_bSuccess = false;
   
   RsyncPacket::Type l_PktType = pPacket->type();
   
   RegTableLock.lock(Mutex::WaitForever);
   
   if (RegistrationTable[l_PktType])
   {
      RegistrationTable[l_PktType]->pushPacket(pPacket);
      l_bSuccess = true;
   }
   
   RegTableLock.unlock();
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool RsyncThread::start()
{
   bool l_bSuccess = true;
   
   if (m_pRsyncThread)
   {
      return false;
   }
   
   m_pRsyncThread = Thread::Create(RsyncThreadFunc, this);
   
   if (m_pRsyncThread == NULL)
   {
      printf("Failed to create rsync thread!\r\n");
      l_bSuccess = false;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
void RsyncThread::stop()
{
   if (m_pRsyncThread)
   {
      m_pRsyncThread->stop();
      m_pRsyncThread->join();
      
      delete m_pRsyncThread;
      m_pRsyncThread = NULL;
   }
}

//------------------------------------------------------------------------------
bool RsyncThread::pushPacket(RsyncPacket* pPacket, ui32 timeoutMs)
{
   return m_InQueue.push(pPacket, timeoutMs);
}

//------------------------------------------------------------------------------
bool RsyncThread::popPacket(RsyncPacket* pPacket, ui32 timeoutMs)
{
   return m_InQueue.pop(pPacket, timeoutMs);
}

//------------------------------------------------------------------------------
void RsyncThread::flushInput()
{
   bool l_bRetVal = false;
   RsyncPacket* l_pTempPkt = NULL;
   
   do {
      
      l_bRetVal = m_InQueue.pop(l_pTempPkt, 100);
      
      if (l_pTempPkt)
      {
         delete l_pTempPkt;
         l_pTempPkt = NULL;
      }
      
   } while (l_bRetVal);
}

//------------------------------------------------------------------------------
void RsyncThread::flushOutput()
{
   bool l_bRetVal = false;
   RsyncPacket* l_pTempPkt = NULL;
   
   do {
      
      l_bRetVal = m_OutQueue.pop(l_pTempPkt, 100);
      
      if (l_pTempPkt)
      {
         delete l_pTempPkt;
         l_pTempPkt = NULL;
      }
      
   } while (l_bRetVal);
}

//------------------------------------------------------------------------------
void RsyncThread::RsyncThreadFunc(ThreadArg* pArg)
{
   ((RsyncThread*)pArg->pUserData)->rsyncThreadFunc(pArg);
}

//------------------------------------------------------------------------------
void RsyncThread::rsyncThreadFunc(ThreadArg* pArg)
{
   while (!pArg->stopSignalled())
   {
   }
}

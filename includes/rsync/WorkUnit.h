#ifndef WORK_UNIT_H
#define WORK_UNIT_H

#include "Queue.h"
#include "RsyncPacket.h"

class WorkUnit
{
public:
   
   WorkUnit(Queue<RsyncPacket*>& outQueue,
            ui32 nInDepth = Queue<RsyncPacket*>::InfiniteQueue);//,
//            ui32 nOutDepth = Queue::InfiniteQueue);
   
   virtual ~WorkUnit();
   
   static void ClearRegistrations();
   static bool Register(RsyncPacket::Type type, WorkUnit* pWorkUnit);
   static bool Unregister(RsyncPacket::Type type);
   static bool Route(RsyncPacket* pPacket);
   
   
   virtual bool work() = 0;
   
   //void setOutputQueue(Queue<RsyncPacket*>& outQueue);
   
   bool pushPacket(RsyncPacket* pPacket, ui32 timeoutMs = 100);
   
//   bool popPacket(RsyncPacket** pPacket, ui32 timeoutMs = 100);
   
protected:
   
   bool popInputPacket(RsyncPacket** pPacket, ui32 timeoutMs = 100);
   
   bool pushOutputPacket(RsyncPacket* pPacket, ui32 timeoutMs = 100);
      
private:
   
   static WorkUnit*  RegistrationTable[RsyncPacket::NumTypes];
   static Mutex      RegTableLock;
   
   Queue<RsyncPacket*> m_inQueue;
//   Queue<RsyncPacket*> m_OutQ;
   Queue<RsyncPacket*>& m_outQueue;
};

#endif // WORK_UNIT_H
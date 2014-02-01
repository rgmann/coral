#include "WorkUnit.h"

WorkUnit* WorkUnit::RegistrationTable[RsyncPacket::NumTypes];
Mutex     WorkUnit::RegTableLock;

//------------------------------------------------------------------------------
WorkUnit::WorkUnit(Queue<RsyncPacket*>& outQueue)
: m_type(RsyncPacket::TypeNotSet)
, m_outQueue(outQueue)
{
}

//------------------------------------------------------------------------------
WorkUnit::~WorkUnit()
{
   // Nothing to deallocate.
   if (m_type != RsyncPacket::TypeNotSet)
   {
      Unregister(m_type);
   }
}

//------------------------------------------------------------------------------
void WorkUnit::ClearRegistrations()
{
   RegTableLock.lock(Mutex::WaitForever);

   for (ui32 l_nType = 0; l_nType < RsyncPacket::NumTypes; l_nType++)
   {
      RegistrationTable[l_nType] = NULL;
   }
   
   RegTableLock.unlock();
}

//------------------------------------------------------------------------------
bool WorkUnit::Register(RsyncPacket::Type type, WorkUnit* pWorkUnit)
{
   bool l_bSuccess = false;
   
   RegTableLock.lock(Mutex::WaitForever);
   
   if (RegistrationTable[type] == NULL)
   {
      RegistrationTable[type] = pWorkUnit;
      l_bSuccess = true;
   }
   
   RegTableLock.unlock();
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool WorkUnit::Unregister(RsyncPacket::Type type)
{
   bool l_bSuccess = false;
   
   RegTableLock.lock(Mutex::WaitForever);
   
   if (RegistrationTable[type] != NULL)
   {
      RegistrationTable[type] = NULL;
      l_bSuccess = true;
   }
   
   RegTableLock.unlock();
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool WorkUnit::Route(RsyncPacket* pPacket)
{
   WorkUnit* l_pWorkUnit = NULL;
   
   if (pPacket == NULL) return false;
   
   RsyncPacket::Type type = pPacket->type();
   
   // See if a WorkUnit has registered for this type of packet.
   RegTableLock.lock(Mutex::WaitForever);
   
   if (RegistrationTable[type] != NULL)
   {
      l_pWorkUnit = RegistrationTable[type];
   }
   
   RegTableLock.unlock();
   
   // The WorkUnit pointer will be NULL if no WorkUnit is regsitered for this
   // packet type.
   if (l_pWorkUnit == NULL) return false;
   
   // Send the packet to the appropriate work unit.
   return l_pWorkUnit->pushPacket(pPacket);
}

//------------------------------------------------------------------------------
bool WorkUnit::initialize(RsyncPacket::Type type, ui32 nInDepth)
{
   bool l_bSuccess = true;
   
   // WorkUnits specify TypeNotSet if they are not registering for a packet
   // type.
   if (type != RsyncPacket::TypeNotSet)
   {
      l_bSuccess = Register(type, this);
      
      if (l_bSuccess) m_type = type;
   }
   
   return (l_bSuccess && m_inQueue.initialize(nInDepth));
}

//------------------------------------------------------------------------------
//void WorkUnit::setOutputQueue(Queue<RsyncPacket*>& outQueue)
//{
//   m_outQueue = outQueue;
//}

//------------------------------------------------------------------------------
bool WorkUnit::pushPacket(RsyncPacket* pPacket, ui32 timeoutMs)
{
   return m_inQueue.push(pPacket, timeoutMs);
}

//------------------------------------------------------------------------------
//bool WorkUnit::popPacket(RsyncPacket** pPacket, ui32 timeoutMs)
//{
//   return m_outQueue.pop(*pPacket, timeoutMs);
//}

//------------------------------------------------------------------------------
bool WorkUnit::popInputPacket(RsyncPacket** pPacket, ui32 timeoutMs)
{
   return m_inQueue.pop(*pPacket, timeoutMs);
}

//------------------------------------------------------------------------------
bool WorkUnit::pushOutputPacket(RsyncPacket* pPacket, ui32 timeoutMs)
{
   return m_outQueue.push(pPacket, timeoutMs);
}

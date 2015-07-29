#include "PacketQueue.h"

using namespace liber;
using namespace liber::netapp;

//----------------------------------------------------------------------------
PacketQueue::PacketQueue()
{
}

//----------------------------------------------------------------------------
PacketQueue::~PacketQueue()
{
}

//----------------------------------------------------------------------------
bool PacketQueue::push(PacketContainer* pContainer)
{
  return mQueue.push(pContainer);
}

//----------------------------------------------------------------------------
PacketContainer* PacketQueue::pop(int nTimeoutMs)
{
  PacketContainer* lpContainer = NULL;
  mQueue.pop(lpContainer, nTimeoutMs);
  return lpContainer;
}


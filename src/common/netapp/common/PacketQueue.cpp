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
bool PacketQueue::send( PacketContainer* container_ptr )
{
  return mQueue.push( container_ptr );
}

//----------------------------------------------------------------------------
PacketContainer* PacketQueue::pop(int nTimeoutMs)
{
  PacketContainer* container_ptr = NULL;
  mQueue.pop( container_ptr, nTimeoutMs );
  return container_ptr;
}


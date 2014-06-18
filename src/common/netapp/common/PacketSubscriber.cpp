#include <string.h>
#include "Log.h"
#include "PacketSubscriber.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
PacketSubscriber::PacketSubscriber()
: mpOutQueue(NULL)
{
}

//-----------------------------------------------------------------------------
PacketSubscriber::~PacketSubscriber()
{
}

//-----------------------------------------------------------------------------
void PacketSubscriber::setId(int id)
{
  mSubscriberId = id;
}

//-----------------------------------------------------------------------------
void PacketSubscriber::setOutputQueue(Queue<NetAppPacket*>* pOutQueue)
{
  mpOutQueue = pOutQueue;
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::sendPacket(GenericPacket* pPacket, i32 nTimeoutMs)
{
  return sendPacketTo(mSubscriberId, pPacket, nTimeoutMs);
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::
sendPacketTo(int destinationID, GenericPacket* pPacket, i32 nTimeoutMs)
{
  bool lbSuccess = false;

  if (mpOutQueue && (mSubscriberId >= 0))
  {
    NetAppPacket* lpPacket = new NetAppPacket(destinationID,
                                              pPacket->allocatedSize());

    memcpy(lpPacket->dataPtr(), pPacket->basePtr(), pPacket->allocatedSize());
    lbSuccess = mpOutQueue->push(lpPacket, nTimeoutMs);
  }

  return lbSuccess;
}


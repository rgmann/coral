#include <string.h>
#include "Timestamp.h"
#include "NetAppPacket.h"
#include "ServerPacketRouter.h"

using namespace liber::net;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
ServerPacketRouter::
ServerPacketRouter(Queue<NetAppPacket*>* pInQueue,
                   Queue<NetAppPacket*>* pOutQueue)
: mpInQueue(pInQueue)
, mpOutQueue(pOutQueue)
{
}

//----------------------------------------------------------------------------
ServerPacketRouter::~ServerPacketRouter()
{
}

//-----------------------------------------------------------------------------
bool ServerPacketRouter::addSubscriber(int               subscriberId,
                                       PacketSubscriber* pSubscriber)
{
  bool lbSuccess = (mSubscriberTable.count(subscriberId) != 0);

  lbSuccess &= (pSubscriber != NULL);
  if (lbSuccess)
  {
    lbSuccess = mTableLock.lock();
    if (lbSuccess)
    {
      pSubscriber->setId(subscriberId);
      pSubscriber->setOutputQueue(mpOutQueue);
      mSubscriberTable.insert(std::make_pair(subscriberId, pSubscriber));
      lbSuccess = true;
      mTableLock.unlock();
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
PacketSubscriber* ServerPacketRouter::removeSubscriber(int subscriberId)
{
  PacketSubscriber* lpSubscriber = NULL;
  bool lbSuccess = (mSubscriberTable.count(subscriberId) != 0);

  lbSuccess &= (lpSubscriber != NULL);
  if (lbSuccess)
  {
    lbSuccess = mTableLock.lock();
    if (lbSuccess)
    {
      lpSubscriber = mSubscriberTable.find(subscriberId)->second;
      mSubscriberTable.erase(subscriberId);
      lbSuccess = true;
      mTableLock.unlock();
    }
  }

  return lpSubscriber;
}

//-----------------------------------------------------------------------------
bool ServerPacketRouter::route(NetAppPacket* pPacket)
{
  bool lbSuccess = false;
  PacketSubscriber* lpSubscriber = NULL;

  if (pPacket)
  {
    int lSubscriberId = pPacket->data()->type;
    lbSuccess = (mSubscriberTable.count(lSubscriberId) != 0);
    if (lbSuccess)
    {
      lbSuccess = mTableLock.lock();
      if (lbSuccess)
      {
        lpSubscriber = mSubscriberTable.find(lSubscriberId)->second;
        lbSuccess = (lpSubscriber != NULL);

        if (lbSuccess)
        {
          lbSuccess = lpSubscriber->put((char*)pPacket->dataPtr(),
                                        pPacket->data()->length);
        }
      }
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void ServerPacketRouter::routeFromQueue(int nSliceTimeMs)
{
  Timestamp lStartTime;
  Timestamp lStopTime;
  NetAppPacket* lpPacket = NULL;

  i64 lnElapsedMs = 0;
  lStartTime.sample();

  while ((lnElapsedMs < nSliceTimeMs) &&
         mpInQueue->pop(lpPacket, mnDequeueTimeoutMs))
  {
    route(lpPacket);
    lStopTime.sample();
    lnElapsedMs = lStopTime.diffInMs(lStartTime);
  }
}


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
: PacketRouter(pOutQueue)
, mpInQueue(pInQueue)
{
}

//----------------------------------------------------------------------------
ServerPacketRouter::~ServerPacketRouter()
{
}

//-----------------------------------------------------------------------------
bool ServerPacketRouter::route(NetAppPacket* pPacket)
{
  bool lbSuccess = false;
  PacketSubscriber* lpSubscriber = NULL;

  if (pPacket)
  {
    int lSubscriberId = pPacket->data()->type;

    lbSuccess = hasSubscriber(lSubscriberId);
    if (lbSuccess)
    {
      lpSubscriber = getSubscriber(lSubscriberId);
      lbSuccess = (lpSubscriber != NULL);

      if (lbSuccess)
      {
//        printf("ServerPacketRouter::route: id = %d, length = %u\n", lSubscriberId, pPacket->data()->length);
        lbSuccess = lpSubscriber->put((char*)pPacket->dataPtr(),
                                      pPacket->data()->length);
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


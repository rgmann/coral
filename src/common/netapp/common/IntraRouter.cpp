#include "Log.h"
#include "IntraRouter.h"

using namespace liber;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
IntraRouter::IntraRouter()
: PacketRouter(&mOutQueue)
, IThread("IntraRouter")
, mpCounterpart(NULL)
{
  mOutQueue.initialize();
}

//-----------------------------------------------------------------------------
IntraRouter::~IntraRouter()
{
}

//-----------------------------------------------------------------------------
void IntraRouter::setCounterpart(IntraRouter* pCounterpart)
{
  mpCounterpart = pCounterpart;
}

//-----------------------------------------------------------------------------
void IntraRouter::run(const bool& bShutdown)
{
  while (!bShutdown)
  {
    NetAppPacket* lpPacket = NULL;

    if (mOutQueue.pop(lpPacket) && lpPacket)
    {
      log::status("IntraRouter - route\n");
      if (mpCounterpart)
      {
        mpCounterpart->routePacket(lpPacket);
      }
      else
      {
        log::error("IntraRouter - no counterpart set\n");
      }

      delete lpPacket;
    }
  }
}

//-----------------------------------------------------------------------------
void IntraRouter::routePacket(NetAppPacket* pPacket)
{
  PacketSubscriber* lpSubscriber = getSubscriber(pPacket->data()->type);
  if (lpSubscriber)
  {
    log::status("IntraRouter - routePacket to subscriber length=%u\n", pPacket->data()->length);
    lpSubscriber->put((char*)pPacket->dataPtr(), pPacket->data()->length);
  }
  else
  {
    log::status("IntraRouter::routePacket - no subscriber gesters for ID=%d\n", pPacket->data()->type);
  }
}



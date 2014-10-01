#include "Log.h"
#include "GenericPacket.h"
#include "PacketSubscriber.h"
#include "IntraRouter.h"

using namespace liber;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
IntraRouter::IntraRouter()
: PacketRouter(&mReceiver)
, IThread("IntraRouter")
, mpCounterpart(NULL)
{
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
    PacketContainer* lpContainer = NULL;
    if ((lpContainer = mReceiver.pop()) != NULL)
    {
      if (mpCounterpart)
      {
        mpCounterpart->routePacket(lpContainer);
      }
      else
      {
        log::error("IntraRouter - no counterpart set\n");
      }

      delete lpContainer;
    }
  }
}

//-----------------------------------------------------------------------------
void IntraRouter::routePacket(PacketContainer* pContainer)
{
  PacketSubscriber* lpSubscriber = getSubscriber(pContainer->mDestinationID);
  if (lpSubscriber)
  {
    lpSubscriber->put((char*)pContainer->mpPacket->basePtr(),
                      pContainer->mpPacket->allocatedSize());
  }
  else
  {
    log::status("IntraRouter::routePacket - no subscriber registered with ID=%d\n",
                pContainer->mDestinationID);
  }
}



#include "Log.h"
#include "RsyncPacket.h"
#include "RsyncPacketRouter.h"

using namespace liber;
using namespace liber::netapp;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
RsyncPacketReceiverHook::RsyncPacketReceiverHook()
: RelayReceiverHook()
, mnSubscriberID(-1)
{
}

//-----------------------------------------------------------------------------
void RsyncPacketReceiverHook::setSubscriberID(int nSubscriberID)
{
  mnSubscriberID = nSubscriberID;
}

//-----------------------------------------------------------------------------
PacketContainer* RsyncPacketReceiverHook::
translate(PacketContainer* pContainer)
{
  PacketContainer* lpContainer = NULL;

  if (mnSubscriberID > 0)
  {
    lpContainer = new PacketContainer();

    lpContainer->mpPacket = new RsyncPacket(pContainer->mDestinationID,
                                            pContainer->mpPacket);
    lpContainer->mDestinationID = mnSubscriberID;
  }
  else
  {
    log::error("RsyncPacketReceiverHook::translate - "
               "RSYNC has not been registered with a valid subscriber ID\n");
  }

  return lpContainer;
}


//-----------------------------------------------------------------------------
RsyncPacketRouter::RsyncPacketRouter()
: PacketRelay(mReceiver)
{
}

//-----------------------------------------------------------------------------
RsyncPacketRouter::~RsyncPacketRouter()
{
}

//-----------------------------------------------------------------------------
void RsyncPacketRouter::setID(int id)
{
  PacketRelay::setID(id);
  mReceiver.setSubscriberID(id);
}

//-----------------------------------------------------------------------------
PacketContainer* RsyncPacketRouter::
toContainer(const char* pData, ui32 nSizeBytes)
{
  PacketContainer* lpContainer = NULL;

  RsyncPacket* lpPacket = new RsyncPacket();
  if ( lpPacket->unpack( pData, nSizeBytes ) )
  {
    lpContainer = new PacketContainer();
    lpContainer->mDestinationID = lpPacket->data()->type;

    RsyncPacket* lpPayload = new RsyncPacket();

    if ( lpPayload->unpack( lpPacket->dataPtr(), lpPacket->data()->length ) )
    {
      lpContainer = new PacketContainer();
      lpContainer->mDestinationID = lpPacket->data()->type;
      lpContainer->mpPacket = lpPayload;
    }
    else
    {
      log::error("toContainer - Failed to unpack payload\n");
      delete lpPayload;
    }
  }
  else
  {
    log::error("RsyncPacketRouter::repackageIn - Failed to unpack packet\n");
    //delete lpPacket;
  }
  delete lpPacket;

  return lpContainer;
}


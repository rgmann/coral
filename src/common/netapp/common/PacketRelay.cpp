#include "Log.h"
#include "GenericPacket.h"
#include "PacketRelay.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
RelayReceiverHook::RelayReceiverHook()
: mpReceiver(NULL)
{
}

//-----------------------------------------------------------------------------
RelayReceiverHook::~RelayReceiverHook()
{
}

//-----------------------------------------------------------------------------
void RelayReceiverHook::setReceiver(PacketReceiver* pReceiver)
{
  mpReceiver = pReceiver;
}

//-----------------------------------------------------------------------------
bool RelayReceiverHook::call(PacketContainer* pContainer)
{
  bool lbSuccess = false;

  if (mpReceiver)
  {
    lbSuccess = mpReceiver->push(translate(pContainer));
  }
  else
  {
    log::error("RelayReceiverHook::call - NULL receiver\n");
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
PacketContainer* RelayReceiverHook::translate(PacketContainer* pContainer)
{
  return pContainer;
}


//-----------------------------------------------------------------------------
PacketRelay::PacketRelay(RelayReceiverHook& rInReceiver)
: PacketRouter(&rInReceiver)
, PacketSubscriber()
, mrInReceiver(rInReceiver)
{
}

//-----------------------------------------------------------------------------
PacketRelay::~PacketRelay()
{
}

//-----------------------------------------------------------------------------
void PacketRelay::setReceiver(PacketReceiver* pReceiver)
{
  PacketSubscriber::setReceiver(pReceiver);
  mrInReceiver.setReceiver(pReceiver);
}

//-----------------------------------------------------------------------------
bool PacketRelay::put(const char* pData, ui32 nSizeBytes)
{
  bool lbSuccess = false;

  PacketContainer* lpContainer = toContainer(pData, nSizeBytes);
  if (lpContainer)
  {
    PacketSubscriber* lpSubscriber = getSubscriber(lpContainer->mDestinationID);
    if (lpSubscriber)
    {
      lbSuccess = lpSubscriber->put((char*)lpContainer->mpPacket->basePtr(),
                                    lpContainer->mpPacket->allocatedSize());

      delete lpContainer->mpPacket;
    }
    else
    {
      log::debug("PacketRelay::put - No subcriber with ID=%d\n",
                 lpContainer->mDestinationID);
    }

    delete lpContainer;
  }
  else
  {
    log::error("PacketRelay::put - Failed to extract relayable container\n");
  }

  return lbSuccess;
}


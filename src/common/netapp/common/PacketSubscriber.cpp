#include <string.h>
#include "Log.h"
#include "GenericPacket.h"
#include "PacketReceiver.h"
#include "PacketSubscriber.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
PacketSubscriber::PacketSubscriber()
: mpReceiver(NULL)
{
}

//-----------------------------------------------------------------------------
PacketSubscriber::~PacketSubscriber()
{
}

//-----------------------------------------------------------------------------
void PacketSubscriber::setID(int id)
{
  mSubscriberId = id;
}

//-----------------------------------------------------------------------------
void PacketSubscriber::setReceiver(PacketReceiver* pReceiver)
{
  mpReceiver = pReceiver;
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::sendPacket(GenericPacket* pPacket)
{
  return sendPacketTo(mSubscriberId, pPacket);
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::
sendPacketTo(int destinationID, GenericPacket* pPacket)
{
  bool lbSuccess = false;

  if (mpReceiver)
  {
    if (mSubscriberId >= 0)
    {
      lbSuccess = mpReceiver->push(new PacketContainer(destinationID, pPacket));
    }
    else
    {
      log::error("PacketSubscriber::sendPacketTo - Invalid subscriber ID=%d\n");
    }
  }
  else
  {
    log::error("PacketSubscriber::sendPacketTo - Null receiver\n");
  }

  return lbSuccess;
}


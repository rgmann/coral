#include <stdlib.h>
#include "GenericPacket.h"
#include "PacketReceiver.h"

using namespace liber::netapp;

//----------------------------------------------------------------------------
PacketContainer::PacketContainer()
: mDestinationID(-1)
, mpPacket(NULL)
{
}

//----------------------------------------------------------------------------
PacketContainer::PacketContainer(int destination, GenericPacket* pPacket)
: mDestinationID(destination)
, mpPacket(pPacket)
{
}


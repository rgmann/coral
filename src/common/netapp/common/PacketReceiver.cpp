#include <stdlib.h>
#include "GenericPacket.h"
#include "PacketReceiver.h"

using namespace liber::netapp;

//----------------------------------------------------------------------------
PacketContainer::PacketContainer()
: destination_id_(-1)
, packet_ptr_(NULL)
{
}

//----------------------------------------------------------------------------
PacketContainer::PacketContainer( DestinationID destination_id, GenericPacket* packet_ptr )
: destination_id_(destination_id)
, packet_ptr_(packet_ptr)
{
}


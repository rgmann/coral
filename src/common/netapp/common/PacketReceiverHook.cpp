#include "PacketReceiverHook.h"

using namespace liber::netapp;

//----------------------------------------------------------------------------
PacketReceiverHook::PacketReceiverHook()
: PacketReceiver()
{
}

//----------------------------------------------------------------------------
PacketReceiverHook::~PacketReceiverHook()
{
}

//----------------------------------------------------------------------------
bool PacketReceiverHook::send( PacketContainer* container_ptr )
{
  return call( container_ptr );
}


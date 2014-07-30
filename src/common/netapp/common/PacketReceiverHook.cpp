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
bool PacketReceiverHook::push(PacketContainer* pContainer)
{
  return call(pContainer);
}


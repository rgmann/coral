#include "Log.h"
#include "GenericPacket.h"
#include "PacketRelay.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
RelayReceiverHook::RelayReceiverHook()
: receiver_ptr_(NULL)
{
}

//-----------------------------------------------------------------------------
RelayReceiverHook::~RelayReceiverHook()
{
}

//-----------------------------------------------------------------------------
void RelayReceiverHook::setReceiver(PacketReceiver* pReceiver)
{
  receiver_ptr_ = pReceiver;
}

//-----------------------------------------------------------------------------
bool RelayReceiverHook::call( PacketContainer* container_ptr )
{
  bool success = false;

  if ( receiver_ptr_ )
  {
    success = receiver_ptr_->push( translate( container_ptr ) );
  }
  else
  {
    log::error("RelayReceiverHook::call - NULL receiver\n");
  }

  return success;
}

//-----------------------------------------------------------------------------
PacketContainer* RelayReceiverHook::translate( PacketContainer* container_ptr )
{
  return container_ptr;
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
   bool success = false;

   PacketContainer* container_ptr = toContainer( pData, nSizeBytes );

   if ( container_ptr )
   {
      PacketSubscriber* subscriber_ptr = getSubscriber(
         container_ptr->mDestinationID
      );

      if ( subscriber_ptr )
      {
         success = subscriber_ptr->put(
            (char*)container_ptr->mpPacket->basePtr(),
            container_ptr->mpPacket->allocatedSize()
         );

         delete container_ptr->mpPacket;
      }
      else
      {
         log::debug("PacketRelay::put - No subcriber with ID=%d\n",
                  container_ptr->mDestinationID);
      }

      delete container_ptr;
   }
   else
   {
     log::error("PacketRelay::put - Failed to extract relayable container\n");
   }

   return success;
}


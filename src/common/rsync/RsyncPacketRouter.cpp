#include "Log.h"
#include "RsyncPacket.h"
#include "RsyncPacketRouter.h"

using namespace liber;
using namespace liber::netapp;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
RsyncPacketReceiverHook::RsyncPacketReceiverHook()
: RelayReceiverHook()
{
}

//-----------------------------------------------------------------------------
PacketContainer* RsyncPacketReceiverHook::translate(
   DestinationID destination_id, PacketContainer* in_container_ptr )
{
   PacketContainer* out_container_ptr = NULL;

   if ( in_container_ptr )
   {
      out_container_ptr = new PacketContainer();

      out_container_ptr->packet_ptr_ = new RsyncTransportPacket(
         in_container_ptr->destination_id_,
         in_container_ptr->packet_ptr_
      );

      out_container_ptr->destination_id_ = destination_id;
   }
   else
   {
      log::error("RsyncPacketReceiverHook::translate - "
                 "RSYNC has not been registered with a valid subscriber ID\n");
   }

   return out_container_ptr;
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
PacketRelay::RelayInfo* RsyncPacketRouter::extract(
   const PacketRelay::RelayInfo& input )
{
   static const ui32 MIN_HEADER_LENGTH = sizeof(RsyncTransportPacket::Data);
   RelayInfo* output = NULL;

   if ( input.data_ptr )
   {
      output = new (std::nothrow) RelayInfo();

      if ( input.data_ptr && ( input.length >= MIN_HEADER_LENGTH ) )
      {
         const RsyncTransportPacket::Data* header_ptr =
            reinterpret_cast<const RsyncTransportPacket::Data*>(input.data_ptr);

         output->destination_id  = header_ptr->type;
         output->length          = header_ptr->length;

         if ( header_ptr->length > MIN_HEADER_LENGTH )
         {
            output->data_ptr = reinterpret_cast<const ui8*>(input.data_ptr) +
                               MIN_HEADER_LENGTH;
         }

         // log::status("RsyncPacketRouter::extract: destination_id=%d, length=%d\n",
         //    output->destination_id, output->length );
      }
      
      if ( output->data_ptr == NULL )
      {
         delete output;
         output = NULL;
      }
   }

   return output;
}


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

      out_container_ptr->packet_ptr_ = new RsyncPacket(
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
   RelayInfo* output = NULL;

   if ( input.data_ptr )
   {
      output = new (std::nothrow) RelayInfo();

      if ( input.data_ptr && ( input.length >= sizeof(RsyncPacket::Data) ) )
      {
         const RsyncPacket::Data* header_ptr =
            reinterpret_cast<const RsyncPacket::Data*>(input.data_ptr);

         output->destination_id = header_ptr->type;
         output->length     = header_ptr->length;

         if ( header_ptr->length > sizeof(RsyncPacket::Data) )
         {
            output->data_ptr = reinterpret_cast<const ui8*>(input.data_ptr) +
                               sizeof(RsyncPacket::Data);
         }
      }
      
      if ( output->data_ptr == NULL )
      {
         delete output;
         output = NULL;
      }
   }

   return output;
}


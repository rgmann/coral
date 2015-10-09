// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#include "Log.h"
#include "GenericPacket.h"
#include "PacketRelay.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
RelayReceiverHook::RelayReceiverHook()
{
}

//-----------------------------------------------------------------------------
RelayReceiverHook::~RelayReceiverHook()
{
}

//-----------------------------------------------------------------------------
bool RelayReceiverHook::registerReceiver( const RegisteredRouter& router_info )
{
   bool add_success = false;

   // Verify that the router is not already in the list.
   RegisteredRouterList::iterator router_iterator = routers_.begin();
   for (; router_iterator != routers_.end(); ++router_iterator )
   {
      if ( router_iterator->destination_id == router_info.destination_id )
         break;
   }

   if ( router_iterator == routers_.end() )
   {
      routers_.push_back( router_info );
      add_success = true;
   }

   return add_success;
}

//-----------------------------------------------------------------------------
void RelayReceiverHook::unregisterReceiver( DestinationID destination_id )
{
   RegisteredRouterList::iterator router_iterator = routers_.begin();
   for (; router_iterator != routers_.end(); ++router_iterator )
   {
      if ( router_iterator->destination_id == destination_id )
      {
         routers_.erase( router_iterator );
         break;
      }
   }
}

//-----------------------------------------------------------------------------
bool RelayReceiverHook::call( PacketContainer* container_ptr )
{
   bool success = false;

   RegisteredRouterList::iterator router_iterator = routers_.begin();
   for (; router_iterator != routers_.end(); ++router_iterator )
   {
      RegisteredRouter& router_info = *router_iterator;

      PacketContainer* translated_container_ptr = translate(
         router_info.destination_id,
         container_ptr
      );

      if ( translated_container_ptr &&
         ( router_iterator->mode == kSubscriberModeReadWrite ) )
      {
         PacketReceiver* receiver_ptr = router_info.receiver_ptr;

         success = receiver_ptr->send( translated_container_ptr );
      }
      else
      {
         // TODO
      }
   }

  return success;
}

//-----------------------------------------------------------------------------
PacketContainer* RelayReceiverHook::translate( DestinationID destination_id, PacketContainer* container_ptr )
{
   return container_ptr;
}


//-----------------------------------------------------------------------------
PacketRelay::PacketRelay(RelayReceiverHook& rInReceiver)
: PacketRouter(&rInReceiver)
, PacketSubscriber()
, relay_receiver_(rInReceiver)
{
}

//-----------------------------------------------------------------------------
PacketRelay::~PacketRelay()
{
}

//-----------------------------------------------------------------------------
PacketReceiver* PacketRelay::setReceiver( RegisteredRouter& registered_router )
{
   PacketReceiver* receiver_ptr = NULL;
   if( relay_receiver_.registerReceiver( registered_router ) )
   {
      receiver_ptr = &relay_receiver_;
   }
  
   return receiver_ptr;
}


//-----------------------------------------------------------------------------
bool PacketRelay::put( DestinationID destination_id, const void* data_ptr, ui32 length )
{
   bool relay_success = false;

   RelayInfo input( destination_id, data_ptr, length );
   RelayInfo* output = extract( input );

   if ( output )
   {
      relay_success = publish( output->destination_id, output->data_ptr, output->length );
      delete output;
   }
   else
   {
      log::error("PacketRelay::put: Failed to extract payload\n");
   }

   return relay_success;
}

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



#include <string.h>
#include "Log.h"
#include "GenericPacket.h"
#include "PacketReceiver.h"
#include "PacketSubscriber.h"

using namespace coral::netapp;

//-----------------------------------------------------------------------------
PacketSubscriber::PacketSubscriber()
{
}

//-----------------------------------------------------------------------------
PacketSubscriber::~PacketSubscriber()
{
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::registerRouter(
   DestinationID destination_id,
   PacketReceiver* receiver_ptr,
   SubscriberMode mode )
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );

   bool success = false;

   if ( routers_.count( destination_id ) == 0 )
   {
      RegisteredRouter router_info;

      router_info.destination_id = destination_id;
      router_info.mode = mode;
      router_info.receiver_ptr = receiver_ptr;

      PacketReceiver* receiver_override_ptr = setReceiver( router_info );
      if ( receiver_override_ptr )
      {
         router_info.receiver_ptr = receiver_override_ptr;
      }

      routers_.insert( std::make_pair( destination_id, router_info ) );

      success = true;
   }

   return success;
}

//-----------------------------------------------------------------------------
PacketReceiver* PacketSubscriber::setReceiver( RegisteredRouter& router_info )
{
   return router_info.receiver_ptr;
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::unregisterRouter( DestinationID destination_id )
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );

   bool success = false;

   RegisteredRouterMap::iterator router_iterator = routers_.find( destination_id );
   if ( router_iterator != routers_.end() )
   {
      routers_.erase( router_iterator );
      success = true;
   }

   return success;
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::isSubscribed() const
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );
   return ( routers_.size() > 0 );
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::sendTo( DestinationID destination_id, GenericPacket* packet_ptr )
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );

   bool success = false;

   if ( packet_ptr && packet_ptr->isAllocated() )
   {
      RegisteredRouterMap::iterator router_iterator = routers_.begin();
      for (; router_iterator != routers_.end(); ++router_iterator )
      {
         PacketContainer* container_ptr = new PacketContainer( destination_id, packet_ptr );

         RegisteredRouter& router_info = router_iterator->second;
         if ( router_info.mode == kSubscriberModeReadWrite )
         {
            PacketReceiver* receiver_ptr = router_info.receiver_ptr;
            if ( receiver_ptr != NULL )
            {
               success = receiver_ptr->send( container_ptr );
            }
         }
      }
   }
   else
   {
      log::error( "PacketSubscriber::sendTo: NULL packet_ptr\n" );
   }

  return success;
}


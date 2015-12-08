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
#include "RpcClient.h"

using namespace coral;
using namespace coral::rpc;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
RpcClient::RpcClient( DestinationID server_destination_id )
   : server_destination_id_( server_destination_id )
{
}

//-----------------------------------------------------------------------------
RpcClient::~RpcClient()
{
   boost::mutex::scoped_lock guard( active_call_table_lock_ );

   CallTable::iterator rpc_iterator = active_calls_.begin();
   while ( rpc_iterator != active_calls_.end() )
   {
      if ( rpc_iterator->second )
      {
         delete rpc_iterator->second;
      }

      active_calls_.erase( rpc_iterator++ );
   }
}

//-----------------------------------------------------------------------------
RpcMarshalledCall* RpcClient::invokeRpc( const RpcObject& object )
{
   RpcMarshalledCall* call_ptr = new (std::nothrow) RpcMarshalledCall(object);

   if ( call_ptr )
   {
      const boost::uuids::uuid& call_id = call_ptr->getRequest().callInfo().uuid;

      if ( active_calls_.count( call_id ) == 0 )
      {
         {
            boost::mutex::scoped_lock guard( active_call_table_lock_ );

            active_calls_.insert( std::make_pair( call_id, call_ptr ) );
         }

         RpcPacket* packet_ptr = call_ptr->toRequestPacket();

         if ( packet_ptr )
         {
            if ( packet_ptr->isAllocated() )
            {
               // The RPC client and server always have the same subscriber ID,
               // so there is no need to send the packet to a particular subscriber ID.
               sendTo( server_destination_id_, packet_ptr );
            }
            else
            {
               delete packet_ptr;
            }
         }
         else
         {
            log::error("RpcClient::invokeRpc: NULL RPC packet\n");
         }
      }
      else
      {
         log::error("RpcClient::invokeRpc - "
                    "RPC call with same UUID already exists\n");
      }
   }
   else
   {
      log::error("RpcClient::invokeRpc - Failed to create RpcMarshalledCall\n");
   }

   return call_ptr;
}

//-----------------------------------------------------------------------------
bool RpcClient::put(
   DestinationID  destination_id,
   const void*    data_ptr,
   ui32           length )
{
   bool success = false;

   RpcPacket packet;
   RpcObject receive_object;

   if ( packet.allocate( data_ptr, length ) )
   {
      if ( packet.getObject( receive_object ) )
      {
         RpcMarshalledCall* call_ptr = NULL;

         const boost::uuids::uuid& call_id = receive_object.callInfo().uuid;

         CallTable::iterator call_iterator = active_calls_.find( call_id );
         if ( call_iterator != active_calls_.end() )
         {
            call_ptr = call_iterator->second;
         }

         if ( call_ptr )
         {
            call_ptr->notify( receive_object );
            success = true;
         }
         else
         {
            log::error("Failed to find RPC with ID\n");
         }
      }
      else
      {
         log::error("RpcClient::put: Failed to get RpcObject\n");
      }
   }
   else
   {
      log::error("RpcClient::put: Failed to unpack RpcPacket\n");
   }

   processCancellations();

   return success;
}

//-----------------------------------------------------------------------------
void RpcClient::processCancellations()
{
   boost::mutex::scoped_lock guard( active_call_table_lock_ );

   CallTable::iterator rpc_iterator = active_calls_.begin();
   while ( rpc_iterator != active_calls_.end() )
   {
      if ( rpc_iterator->second->isDisposed() )
      {
         if (rpc_iterator->second)
         {
            delete rpc_iterator->second;
         }

         active_calls_.erase(rpc_iterator++);
      }
      else
      {
         ++rpc_iterator;
      }
   }
}


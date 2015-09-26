#include "Log.h"
#include "RpcClient.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
RpcClient::RpcClient( DestinationID server_destination_id )
   : server_destination_id_( server_destination_id )
{
}

//-----------------------------------------------------------------------------
RpcClient::~RpcClient()
{
   boost::mutex::scoped_lock guard( mRpcMutex );

   CallMap::iterator rpc_iterator = mRpcMap.begin();
   while ( rpc_iterator != mRpcMap.end() )
   {
      if ( rpc_iterator->second )
      {
         delete rpc_iterator->second;
      }

      mRpcMap.erase( rpc_iterator++ );
   }
}

//-----------------------------------------------------------------------------
RpcMarshalledCall* RpcClient::invokeRpc(const RpcObject &object)
{
   RpcMarshalledCall* call_ptr = new (std::nothrow) RpcMarshalledCall(object);

   if ( call_ptr )
   {
      const boost::uuids::uuid& call_id = call_ptr->input().callInfo().uuid;

      if ( mRpcMap.count( call_id ) == 0 )
      {
         {
            boost::mutex::scoped_lock guard( mRpcMutex );

            mRpcMap.insert( std::make_pair( call_id, call_ptr ) );
         }

         RpcPacket* packet_ptr = call_ptr->getRpcPacket();

         if ( packet_ptr )
         {
            if ( packet_ptr->isAllocated() )
            {
               log::status("RpcClient::invokeRpc: Send request - %d\n", packet_ptr->allocatedSize() );
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
bool RpcClient::put( DestinationID destination_id, const void* data_ptr, ui32 length )
{
   bool success = false;

   RpcPacket packet;
   RpcObject receive_object;

   if ( packet.unpack( data_ptr, length ) )
   {
      if ( packet.getObject( receive_object ) )
      {
         RpcMarshalledCall* call_ptr = NULL;

         const boost::uuids::uuid& call_id = receive_object.callInfo().uuid;

         CallMap::iterator call_iterator = mRpcMap.find( call_id );
         if ( call_iterator != mRpcMap.end() )
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
   boost::mutex::scoped_lock guard( mRpcMutex );

   CallMap::iterator rpc_iterator = mRpcMap.begin();
   while ( rpc_iterator != mRpcMap.end() )
   {
      if ( rpc_iterator->second->isDisposed() )
      {
         if (rpc_iterator->second)
         {
            delete rpc_iterator->second;
         }

         mRpcMap.erase(rpc_iterator++);
      }
      else
      {
         ++rpc_iterator;
      }
   }
}


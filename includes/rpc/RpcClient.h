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



#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/uuid/uuid.hpp>
#include "Queue.h"
#include "RpcPacket.h"
#include "RpcMarshalledCall.h"
#include "PacketSubscriber.h"

namespace coral {
namespace rpc {


///
/// RPC client hub:
/// The RpcClient manages active RPC invocations, multiplexes requests to the
/// server-side services, and de-multiplexes server-side service responses.
/// Multiple RPC services/resources may be register with a single client.
///
class RpcClient : public coral::netapp::PacketSubscriber {
public:

   ///
   /// Construct an RpcClient instance with the destination ID of
   /// of the corresponding RpcServer.
   ///
   RpcClient( coral::netapp::DestinationID server_destination_id );
   
   ~RpcClient();

   ///
   /// Initiate the remote procedure call with the specified request object.
   /// On success, an RpcMarshalledCall is created and registered to maintain
   /// status while the call is active
   ///
   /// @param  object  RPC request object
   /// @return RpcMarshalledCall*  Pointer to RPC context
   ///
   RpcMarshalledCall* invokeRpc( const RpcObject& object );

   ///
   /// Route a packet to the RpcClient.  This method implements the
   /// PacketSubscriber interface and is only intended to be invoked by a
   /// PacketRouter.
   ///
   /// @param  destination_id  Packet destination ID
   /// @param  data_ptr        Packet data buffer
   /// @param  length          Packet data buffer length in bytes
   /// @return bool            Packet processing status
   ///
   bool put( coral::netapp::DestinationID destination_id, const void* data_ptr, ui32 length );


private:

   ///
   /// An RpcClient cannot be copied
   ///
   RpcClient( const RpcClient& );
   RpcClient& operator= ( const RpcClient& );

   ///
   /// Remove completed or canceled RPC calls from the call table.
   ///
   void processCancellations();

private:

   boost::mutex active_call_table_lock_;
   
   // Map each call to a unique identifier so that we can look up the
   // call when a response is received.
   typedef std::map<boost::uuids::uuid, RpcMarshalledCall*> CallTable;
   CallTable active_calls_;

   coral::netapp::DestinationID server_destination_id_;
};

} // end namespace rpc
} // end namespace coral

#endif // RPC_CLIENT_H

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

namespace liber {
namespace rpc {

class RpcClient : public liber::netapp::PacketSubscriber {
public:

   RpcClient( liber::netapp::DestinationID server_destination_id );
   
   ~RpcClient();

   RpcMarshalledCall* invokeRpc(const RpcObject &object);

   bool put( liber::netapp::DestinationID destination_id, const void* data_ptr, ui32 length );

private:

   RpcMarshalledCall* getMarshalledRpc( const RpcObject &object );

   void processCancellations();

private:

   boost::mutex mRpcMutex;
   
   // Map each call to a unique identifier so that we can look up the
   // call when a response is received.
   typedef std::map<boost::uuids::uuid, RpcMarshalledCall*> CallMap;
   CallMap mRpcMap;

   liber::netapp::DestinationID server_destination_id_;
};

}}

#endif // RPC_CLIENT_H

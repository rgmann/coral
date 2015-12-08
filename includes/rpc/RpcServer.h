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



#ifndef RPC_SERVER_H
#define RPC_SERVER_H

#include <map>
#include <string>
#include "RpcPacket.h"
#include "RpcServerResource.h"
#include "PacketSubscriber.h"

namespace coral {
namespace rpc {

///
/// RPC server hub:
/// The server maintains a collection of registered resources/services.
///
class RpcServer : public coral::netapp::PacketSubscriber {
public:

   ///
   /// Construct an RpcServer instance with the destination ID of
   /// of the corresponding RpcClient.
   ///
   /// @param  client_destination_id  Destination ID of RpcClient
   ///
   RpcServer( coral::netapp::DestinationID client_destination_id );

   ///
   /// Register a resource/service
   ///
   bool registerResource( RpcServerResource* resource_ptr );

   ///
   /// Route a packet to the server for routing to the associated resource.
   /// This method implements the PacketSubscriber interface.
   ///
   /// @param  destination_id  ID of packet sender
   /// @param  data_ptr        Pointer to packet buffer
   /// @param  length          Size of packet buffer in bytes
   /// @return bool            True on success; false on failure
   ///
   bool put( coral::netapp::DestinationID, const void* data_ptr, ui32 length );


private:

   ///
   /// Process a request packet.
   ///
   /// @param  packet_ptr  Pointer to request packet
   /// @return bool        True if the packet was successfully processed;
   ///                     false otherwise
   ///
   bool processRequest( const RpcPacket* packet_ptr );

   ///
   /// Find the resource / service associated with a request.
   ///
   /// @param  object  Request object
   /// @return RpcServerResource*  RPC server resource or NULL
   ///
   RpcServerResource* getResource( const RpcObject& object );

   ///
   /// Send a response object
   ///
   /// @param  object  Response object
   /// @return void
   ///
   void sendObject( const RpcObject& object );


private:

   ///
   /// Copies are not permitted.
   ///
   RpcServer( const RpcServer& );
   RpcServer& operator= ( const RpcServer& );


private:

   typedef  std::map<std::string, RpcServerResource*>  ResourceMap;

   ResourceMap resources_;

   coral::netapp::DestinationID client_destination_id_;
};

} // end namespace rpc
} // end namesapce coral

#endif // RPC_SERVER_H

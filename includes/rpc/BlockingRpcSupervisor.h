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



#ifndef BLOCKING_RPC_SUPERVISOR_H
#define BLOCKING_RPC_SUPERVISOR_H

#include "RpcSupervisor.h"

namespace coral {
namespace rpc {

///
/// The BlockingRpcSupervisor blocks the caller until the remote procedure call
/// has completed or timed out.
///
class BlockingRpcSupervisor : public RpcSupervisor {
public:

   BlockingRpcSupervisor();
   ~BlockingRpcSupervisor();

   ///
   /// Invoke the remote procedure call.
   ///
   /// @param  client  RpcClient through which the request should be sent
   /// @param  request Request object
   /// @param  response_message_ptr Deserialized response message buffer
   /// @param  timeout_ms  Call timeout in milliseconds
   /// return  bool    True on success; false on failure
   ///
   bool invoke( RpcClient&       client,
                const RpcObject& request,
                PbMessage*       response_message_ptr,
                int              timeout_ms );


private:

   ///
   /// Copies are not permitted.
   ///
   BlockingRpcSupervisor( const BlockingRpcSupervisor& );
   BlockingRpcSupervisor& operator= ( const BlockingRpcSupervisor& );

}; // End class BlockingRpcSupervisor

} // End namespace rpc
} // End namespace coral

#endif // BLOCKING_RPC_SUPERVISOR_H

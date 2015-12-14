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


#ifndef RPC_SUPERVISOR_H
#define RPC_SUPERVISOR_H

#include "RpcClient.h"

namespace coral {
namespace rpc {

///
/// A supervisor waits for a remote procedure call to complete or timeout.
/// The RpcSupervisor is an abstract base class that provides a common
/// interface for the BlockingRpcSupervisor and the AsyncRpcSupervisor.
///
class RpcSupervisor {
public:

   RpcSupervisor() {};
   virtual ~RpcSupervisor() {};

   ///
   /// Invoke the remote procedure call. This method must be implemented by
   /// derived supervisors.
   ///
   /// @param  client  RpcClient through which the request should be sent
   /// @param  request Request object
   /// @param  response_message_ptr Deserialized response message buffer
   /// @param  timeout_ms  Call timeout in milliseconds
   /// return  bool    True on success; false on failure
   ///
   virtual bool invoke( RpcClient&       client,
                        const RpcObject& request,
                        PbMessage*       response_message_ptr,
                        int              timeout_ms ) = 0;

   ///
   /// Get the exception associated with this request.
   ///
   /// @return RpcException  RPC exception
   ///
   RpcException& exception();

   ///
   /// Get the remote procedure call response.
   ///
   /// @return RpcObject  Response object
   ///
   RpcObject& response();


private:
  
   ///
   /// Copies are not permitted.
   ///
   RpcSupervisor( const RpcSupervisor& );
   RpcSupervisor& operator= (const RpcSupervisor&);


protected:

  RpcObject    response_object_;

  RpcException exception_;

}; // End class RpcSupervisor

} // End namespace rpc
} // End namespace coral

#endif // RPC_SUPERVISOR_H

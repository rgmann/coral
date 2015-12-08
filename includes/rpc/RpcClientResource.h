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



#ifndef RPC_CLIENT_RESOURCE_H
#define RPC_CLIENT_RESOURCE_H

#include <string>
#include "RpcObject.h"
#include "RpcException.h"
#include "RpcClient.h"
#include "AsyncRpcSupervisor.h"


namespace coral {
namespace rpc {

///
/// Abstract base class for all client-side service stubs, or client-side
/// resources.
///
/// The RpcClientResource is not thread safe.
///
class RpcClientResource {
public:

   virtual ~RpcClientResource();
   
   ///
   /// Set call timeout in milliseconds.
   ///
   /// @param  timeout_ms  Timeout in milliseconds
   /// @return void
   ///
   void setTimeout( int timeout_ms );

   ///
   /// Get the last error encountered by this client resource.
   ///
   /// @return RpcException  Last error
   ///
   RpcException getLastError();


protected:

   ///
   /// Construct a client resource
   ///
   /// @param  client  RpcClient through which all remote procedure calls are issued
   /// @param  resource_name  Name of resource
   ///
   RpcClientResource( RpcClient& client, const std::string& resource_name );


   ///
   /// Call the named remote method.  If an AsyncRpcSupervisor is passed,
   /// then the request will be transmitted and call() will return immediately.
   /// Otherwise, call() blocks until a response is received, the call times
   /// out, or until some other error occurs.
   ///
   /// @param  method_name  Remote procedure name
   /// @param  request      Remote procedure parameter message
   /// @param  response     Remote procedure response message
   /// @param  supervisor   Optional asynchronous call supervisor
   /// @return bool         True on success; false on failure. On failure,
   ///                      GetLastError() should be uses to get more
   ///                      information about the error condition.
   ///   
   bool call(const std::string&  methodName,
             const PbMessage&    request,
             PbMessage&          response,
             AsyncRpcSupervisor* supervisor = NULL);

   ///
   /// Given a method name and request-parameter message, build a request
   /// object.
   ///
   /// @param  request_object Marshalled request object
   /// @param  method_name    Remote procedure name
   /// @param  request_params Remote procedure parameter message
   /// @return void
   ///
   void marshallRequest(RpcObject&         request_object,
                        const std::string& method_name,
                        const PbMessage*   request_params = NULL);


private:

   ///
   /// Copies are not permitted
   ///
   RpcClientResource( const RpcClientResource& );
   RpcClientResource& operator= ( const RpcClientResource& );


protected:

   RpcClient& rpc_client_;
   
   std::string resource_name_;
   
   RpcException last_error_;

   int timeout_ms_;
};

} // end namespace rpc
} // end namespace coral

#endif // RPC_CLIENT_RESOURCE_H

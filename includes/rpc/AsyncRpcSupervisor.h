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



#ifndef ASYNC_RPC_SUPERVISOR_H
#define ASYNC_RPC_SUPERVISOR_H

#include <boost/thread/mutex.hpp>
#include "IThread.h"
#include "RpcSupervisor.h"

namespace coral {
namespace rpc {

///
/// The AsyncRpcSupervisor waits for a remote procedure call to complete or
/// timeout asynchronous of the thread that invoked the call. An AsyncRpcSupervisor
/// cannot be directly instantiated. Rather, it should be derived and the 
/// "callback" method should be implemented by the inheriting class.
///
class AsyncRpcSupervisor : public RpcSupervisor, public coral::thread::IThread {
public:

   AsyncRpcSupervisor();
   virtual ~AsyncRpcSupervisor();

   ///
   /// Callback method invoked on completion, timeout, or if any other error
   /// occurs. Derived classes can obtain call completion status via the
   /// RpcSupervisor accessors and via the accessors below.
   ///
   /// @return void
   ///
   virtual void callback() = 0;

   ///
   /// Cancel an active request.
   ///
   /// @return void
   ///
   void cancel();

   ///
   /// Get status indicating whether the call completed because it was canceled.
   /// A valid response is not available.
   ///
   /// @return bool  True if the call completed due to cancellation;
   ///               false otherwise.
   ///
   bool cancelled() const;

   ///
   /// Get status indicating whether a call is active.
   ///
   /// @return bool  A remote procedure call is currently active;
   ///               false otherwise.
   ///
   bool isBusy() const;

   ///
   /// Reset the supervisor that it may used for a new call. In order to be
   /// reset, the supervisor must not be busy.
   ///
   /// @return bool  True if the supervisor was reset; false if it could not be
   ///               reset because it is bsuy.
   ///
   bool reset();

   ///
   /// Get status indicating whether the remote procedure call completed
   /// successfully or failed. Failure details is accessible via the base class
   /// exception accessor.
   ///
   /// @return bool  True if the call completed due a failure; false otherwise.
   ///
   bool failed() const;

   ///
   /// Invoke the remote procedure call.
   ///
   /// @param  client  RpcClient through which the request should be sent
   /// @param  request Request object
   /// @param  response_message_ptr Deserialized response message buffer
   /// @param  timeout_ms  Call timeout in milliseconds
   /// return  bool    True on success; false on failure
   ///
   bool invoke( RpcClient&      client,
                const RpcObject& request,
                PbMessage*       response_message_ptr,
                int              timeout_ms );

private:

   ///
   /// Copies are not permitted.
   ///
   AsyncRpcSupervisor( const AsyncRpcSupervisor& );
   AsyncRpcSupervisor& operator= ( const AsyncRpcSupervisor& );

   ///
   /// Thread loop
   ///
   /// @param  shutdown  Thread shutdown is being requested
   ///
   void run( const bool& shutdown );

private:

   mutable boost::mutex call_lock_;

   bool               cancelled_;
   RpcMarshalledCall* marshalled_call_;
   PbMessage*         response_message_ptr_;
   int                timeout_ms_;

}; // End class AsyncRpcSupervisor

} // End namespace rpc
} // End namespace coral

#endif // ASYNC_RPC_SUPERVISOR_H

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


#ifndef RPC_MARSHALLED_CALL_H
#define RPC_MARSHALLED_CALL_H

#include <time.h>
#include "BinarySem.h"
#include "RpcObject.h"
#include "RpcPacket.h"

namespace coral {
namespace rpc {

///
/// The RpcMarshalledCall class maintains the state of an active remote
/// procedure call.
///
class RpcMarshalledCall {
public:

   // RpcMarshalledCall();
   
   RpcMarshalledCall( const RpcObject &object );

   const RpcObject& getRequest() const;

   ///
   /// Allocate an RpcPacket to encapsulate the request.
   ///
   /// @return RpcPacket*  New RpcPacket object
   ///
   RpcPacket* toRequestPacket() const;

   ///
   /// Get the response to this call.
   ///
   /// @return RpcObject&  Call response object
   ///
   const RpcObject& getResponse() const;

   ///
   /// Notify the thread waiting on call completion that a response has been
   /// received.
   ///
   /// @param  response  Call response object
   /// @return void
   ///
   void notify( const RpcObject& response );

   ///
   /// Cancel the call.
   ///
   /// @return void
   ///
   void cancel();

   ///
   /// Query whether the call has been canceled.
   ///
   /// @return bool  True if the call has been canceled 
   ///
   bool canceled() const;

   ///
   /// Wait for a response to the remote procedure call or for the timeout to
   /// expire.
   ///
   /// @param  timeout_ms  Call timeout period in milliseconds
   /// @param  cancel_on_timeout  True if the call should be disposed on a
   ///               timeout
   /// @return bool  True if a response was received before the timeout;
   ///               false otherwise
   ///
   bool wait( ui32 timeout_ms, bool cancel_on_timeout = true );
   
   ///
   /// Indicate that the caller is no longer waiting on this call. This call
   /// can be removed from the active call table.
   ///
   /// @return void
   ///
   void dispose();

   ///
   /// Query whether this call has been disposed.  A disposed call may be
   /// removed from the active call table.
   ///
   /// @return bool  True if this call is disposed; false otherwise
   ///
   bool isDisposed() const;


private:

   ///
   /// Copying is not permitted
   ///
   RpcMarshalledCall( const RpcMarshalledCall& );
   RpcMarshalledCall& operator= ( const RpcMarshalledCall& );


private:
   
   thread::BinarySem response_sem_;

   RpcObject request_object_;
   RpcObject response_object_;
 
   bool is_disposed_;
   bool canceled_;
};

} // end namespace rpc
} // end namespace coral

#endif // RPC_MARSHALLED_CALL_H

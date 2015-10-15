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

class RpcMarshalledCall {
public:

   RpcMarshalledCall();
   
   RpcMarshalledCall(const RpcObject &object);
   
   RpcPacket* getRpcPacket() const;

   void getResult(RpcObject &result);

   const RpcObject& input() const;

   void notify(const RpcObject &object);
   void cancel();
   bool cancelled() const;

   bool wait( ui32 timeout_ms, bool cancel_on_timeout = true );
   
   // Indicates that the caller is no longer waiting.  This call can be
   // garbage collected.
   void dispose();
   bool isDisposed() const;
   
   RpcObject& getResultObject();

private:
   
   BinarySem mSem;

   RpcObject param_object_;
   RpcObject result_object_;
 
   bool is_disposed_;
   bool cancelled_;
};

}}

#endif // RPC_MARSHALLED_CALL_H

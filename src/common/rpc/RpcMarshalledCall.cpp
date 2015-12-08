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



#include <iostream>
#include "Log.h"
#include "RpcMarshalledCall.h"

using namespace coral::rpc;
using namespace coral::thread;


//-----------------------------------------------------------------------------
RpcMarshalledCall::RpcMarshalledCall( const RpcObject &object )
   : is_disposed_( false )
   , canceled_( false )
{
   request_object_ = object;
}

//-----------------------------------------------------------------------------
RpcPacket* RpcMarshalledCall::toRequestPacket() const
{
   return new (std::nothrow) RpcPacket( request_object_ );
}

//-----------------------------------------------------------------------------
const RpcObject& RpcMarshalledCall::getResponse() const
{
   return response_object_;
}

//-----------------------------------------------------------------------------
const RpcObject& RpcMarshalledCall::getRequest() const
{
   return request_object_;
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::dispose()
{
   is_disposed_ = true;
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::isDisposed() const
{
   return is_disposed_;
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::notify(const RpcObject &object)
{
   response_object_ = object;
   response_sem_.give();
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::cancel()
{
   canceled_ = true;
   response_sem_.give();
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::canceled() const
{
  return canceled_;
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::wait( ui32 timeout_ms, bool cancel_on_timeout )
{
   bool success = ( response_sem_.take( timeout_ms ) == Semaphore::SemAcquired );

   if ( success && cancel_on_timeout )
   {
      is_disposed_ = true;
   }

   return success;
}



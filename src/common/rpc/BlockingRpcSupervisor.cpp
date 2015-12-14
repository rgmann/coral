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



#include "BlockingRpcSupervisor.h"
#include "Log.h"

using namespace coral;
using namespace coral::rpc;

//----------------------------------------------------------------------------
BlockingRpcSupervisor::BlockingRpcSupervisor()
   : RpcSupervisor()
{
}

//----------------------------------------------------------------------------
BlockingRpcSupervisor::~BlockingRpcSupervisor()
{
}

//----------------------------------------------------------------------------
bool BlockingRpcSupervisor::invoke(
   RpcClient&        rpc_client,
   const RpcObject&  request,
   PbMessage*        response_message_ptr,
   int               timeout_ms )
{
   exception_.pushFrame( TraceFrame(
      "BlockingRpcSupervisor",
      "invoke",
      __FILE__,
      __LINE__
   ));

   // Send the marshalled RPC to the RpcClient.
   RpcMarshalledCall* call_ptr = rpc_client.invokeRpc( request );

   if ( call_ptr->wait( timeout_ms ) )
   {
      response_object_ = call_ptr->getResponse();

      if ( response_object_.exception().id == kNoException )
      {
         if ( response_message_ptr )
         {
            response_object_.getParams( *response_message_ptr );
         }
      }
      else
      {
         exception_.pushTrace( response_object_.exception() );
      }
   }
   else
   {
      exception_.reporter = RpcException::kClient;
      exception_.id       = kRpcCallTimeout;
      exception_.message  = "Time out elapsed waiting for resource response.";
   }

   call_ptr->dispose();
   exception_.popFrame();

   return ( exception_.id == kNoException );
}



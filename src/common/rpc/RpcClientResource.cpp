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



#include <boost/uuid/nil_generator.hpp>
#include "Log.h"
#include "RpcClientResource.h"
#include "BlockingRpcSupervisor.h"

using namespace liber;
using namespace liber::rpc;

//-----------------------------------------------------------------------------
RpcClientResource::RpcClientResource(
   RpcClient&         client,
   const std::string& classname
)
   : mrClient(client)
   , mClassname(classname)
   , mnTimeoutMs(3000)
{
}

//-----------------------------------------------------------------------------
RpcClientResource::~RpcClientResource()
{
}

//-----------------------------------------------------------------------------
void RpcClientResource::setTimeout(int nTimeoutMs)
{
  mnTimeoutMs = nTimeoutMs;
}

//-----------------------------------------------------------------------------
RpcException RpcClientResource::getLastError()
{
   return mLastError;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::call(const std::string&  action,
                             const PbMessage&    request,
                             PbMessage&          response,
                             AsyncRpcSupervisor* async_supervisor_ptr )
{
   bool call_success = false;
   RpcObject request_object;

   BlockingRpcSupervisor blocking_supervisor;
   RpcSupervisor* supervisor_ptr = &blocking_supervisor;

   mLastError.reset();
   mLastError.pushFrame( TraceFrame(
      "RpcClientResource", "call",
      __FILE__, __LINE__
   ));

   if ( async_supervisor_ptr != NULL )
   {
     supervisor_ptr = async_supervisor_ptr;
   }

   marshallRequest( request_object, action, &request );
   
   call_success = supervisor_ptr->invoke(
      mrClient, 
      request_object,
      &response,
      mnTimeoutMs
   );

   if ( call_success == false )
   {
      mLastError.pushTrace( supervisor_ptr->exception() );
   }

   mLastError.popFrame();
   
   return call_success;
}

//-----------------------------------------------------------------------------
void RpcClientResource::marshallRequest(
   RpcObject&         requestObject,
   const std::string& methodName,
   const PbMessage*   pRequestParameters)
{
   requestObject.callInfo().resource = mClassname;
   // requestObject.callInfo().uuid = mUuid;
   requestObject.callInfo().action   = methodName;
   if ( pRequestParameters )
   {
      requestObject.setParams( *pRequestParameters );
   }
}

//-----------------------------------------------------------------------------
bool RpcClientResource::invoke(
   const RpcObject& object,
   RpcObject&       result,
   ui32             nTimeoutMs)
{
   bool success = false;

   // Send the marshalled RPC to the RpcClient.
   RpcMarshalledCall* marshalled_call = mrClient.invokeRpc( object );

   if ( ( success = marshalled_call->wait( nTimeoutMs ) ) == true )
   {
      marshalled_call->getResult( result );
   }

   marshalled_call->dispose();

   return success;
}


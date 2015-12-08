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

using namespace coral;
using namespace coral::rpc;

//-----------------------------------------------------------------------------
RpcClientResource::RpcClientResource(
   RpcClient&         client,
   const std::string& resource_name
)
   : rpc_client_( client )
   , resource_name_( resource_name )
   , timeout_ms_( 3000 )
{
}

//-----------------------------------------------------------------------------
RpcClientResource::~RpcClientResource()
{
}

//-----------------------------------------------------------------------------
void RpcClientResource::setTimeout(int nTimeoutMs)
{
  timeout_ms_ = nTimeoutMs;
}

//-----------------------------------------------------------------------------
RpcException RpcClientResource::getLastError()
{
   return last_error_;
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

   last_error_.reset();
   last_error_.pushFrame( TraceFrame(
      "RpcClientResource", "call",
      __FILE__, __LINE__
   ));

   if ( async_supervisor_ptr != NULL )
   {
     supervisor_ptr = async_supervisor_ptr;
   }

   marshallRequest( request_object, action, &request );
   
   call_success = supervisor_ptr->invoke(
      rpc_client_, 
      request_object,
      &response,
      timeout_ms_
   );

   if ( call_success == false )
   {
      last_error_.pushTrace( supervisor_ptr->exception() );
   }

   last_error_.popFrame();
   
   return call_success;
}

//-----------------------------------------------------------------------------
void RpcClientResource::marshallRequest(
   RpcObject&         request_object,
   const std::string& method_name,
   const PbMessage*   request_params )
{
   request_object.callInfo().resource = resource_name_;
   request_object.callInfo().action   = method_name;

   if ( request_params )
   {
      request_object.setParams( *request_params );
   }
}


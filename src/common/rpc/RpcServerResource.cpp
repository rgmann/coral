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



#include <utility>
#include <iostream>
#include <sstream>
#include <boost/uuid/random_generator.hpp>
#include "Log.h"
#include "Timestamp.h"
#include "RpcServerResource.h"

using namespace liber;
using namespace liber::rpc;

//------------------------------------------------------------------------------
RpcServerResource::RpcServerResource(const std::string &name)
   : resource_name_(name)
{
}

//------------------------------------------------------------------------------
std::string RpcServerResource::getName() const
{
   return resource_name_;
}

//------------------------------------------------------------------------------
bool RpcServerResource::unmarshall( RpcObject &input, RpcObject &output )
{
   bool unmarshall_success = true;
   
   boost::uuids::uuid call_uuid = input.callInfo().uuid;

   input.exception().pushFrame( TraceFrame(
      "RpcServerResource",
      "unmarshall",
      __FILE__,
      __LINE__
   ));

   log::status("RpcServerResource::unmarshall: %d\n", input.isValid() );
   if ( input.isValid() )
   {
      unmarshall_success = invoke( call_uuid, input, output );
      input.exception().popFrame();
   }
   else
   {
      exception( input, output, MissingParameters );
   }
   
   return unmarshall_success;
}

//------------------------------------------------------------------------------
bool RpcServerResource::invoke(
   boost::uuids::uuid&  uuid,
   RpcObject&           input,
   RpcObject&           output
)
{
   bool invoke_success = false;

   input.exception().pushFrame( TraceFrame(
      "RpcServerResource",
      "invoke",
      __FILE__,
      __LINE__
   ));

   ActionMap::iterator method_iterator = actions_.find( input.callInfo().action );

   if ( method_iterator != actions_.end() )
   {
      log::status("RpcServerResource::invoke: Found action\n");
      std::string input_params;
      std::string output_params;

      RpcServiceAction* action_ptr = method_iterator->second;

      input.getParams( input_params );

      if ( action_ptr )
      {
         (*action_ptr)( input_params, output_params, input.exception() );

         input.getResponse( output, output_params );

         invoke_success = true;
      }
      else
      {
         exception(
            input, output,
            NullAction,
            "Null reference for \"" + input.callInfo().action + "\" action"
         );

         invoke_success = false;
      }
   }
   else
   {
      log::error("RpcServerResource::invoke: No action registered\n");
   }

   input.exception().popFrame();
   
   return invoke_success;
}

//------------------------------------------------------------------------------
bool RpcServerResource::addAction( RpcServiceAction* action_ptr )
{
   bool add_action_success = false;
   
   if ( actions_.count( action_ptr->getName() ) != 0 )
   {
      actions_.erase( action_ptr->getName() );
   }

   {
      std::pair<ActionMap::iterator,bool> add_status =
         actions_.insert( std::make_pair( action_ptr->getName(), action_ptr ) );

      add_action_success = add_status.second;
   }
   
   return add_action_success;
}

//------------------------------------------------------------------------------
void RpcServerResource::exception(
   RpcObject&         request,
   RpcObject&         response,
   RpcErrorId         eid,
   const std::string& message
)
{
   log::error("RpcServerResource::exception: error - %d\n", eid);
   request.exception().reporter = RpcException::Server;
   request.exception().id       = eid;
   request.exception().message  = message;

   request.getResponse( response );
}


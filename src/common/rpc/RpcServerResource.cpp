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


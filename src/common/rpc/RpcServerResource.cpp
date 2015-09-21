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
   , mnInstanceCount(0)
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
      __FILE__, __LINE__));


   if ( input.isValid() )
   {
      if ( input.callInfo().action == "construct" )
      {
         unmarshall_success = construct( input, output );
      }
      else if ( input.callInfo().action == "destroy" )
      {
         unmarshall_success = destroy( input, output );
      }
      else
      {
         if ( isValidInstance( call_uuid ) )
         {
            unmarshall_success = invoke( call_uuid, input, output );
         }
         else
         {
            exception( input, output, InvalidUIID, "Invalid instance UIID" );
            unmarshall_success = false;
         }
      }

      input.exception().popFrame();
   }
   else
   {
      exception( input, output, MissingParameters );
   }
   
   return unmarshall_success;
}

//------------------------------------------------------------------------------
bool RpcServerResource::isValidInstance( boost::uuids::uuid& uuid )
{
   return ( getInstance( uuid ) != NULL);
}

//------------------------------------------------------------------------------
bool RpcServerResource::construct(RpcObject &input, RpcObject &output)
{
   bool construct_success = false;

   std::string parameter_list;

   input.exception().pushFrame( TraceFrame(
      "RpcServerResource",
      "construct",
      __FILE__,
      __LINE__
   ));

   boost::uuids::uuid instance_uuid = boost::uuids::random_generator()();

   if ( instances_.count( instance_uuid ) == 0 )
   {
      input.getParams( parameter_list );

      InstanceWrapper* wrapper_ptr = createInstance();

      if ( wrapper_ptr == NULL )
      {
         exception(input, output, NullInstance,
                   "Failed to instantiate new \"" +
                   input.callInfo().resource + "\" resource");
      }
      else
      {
         instances_.insert( std::make_pair( instance_uuid, wrapper_ptr ) );

         construct_success = wrapper_ptr->initialize( parameter_list );
         tugCtorHook( wrapper_ptr );
       
         input.getResponse( output );
         output.callInfo().uuid = instance_uuid;

         mnInstanceCount++;

         input.exception().popFrame();
      }
   }
   else
   {
      exception( input, output, UIIDAssignmentErr );
   }

   return construct_success;
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

   MethodMap::iterator method_iterator = methods_.find( input.callInfo().action );

   if ( method_iterator != methods_.end() )
   {
      std::string input_params;
      std::string output_params;

      InstanceWrapper::Method wrapper = method_iterator->second;

      input.getParams( input_params );

      if ( wrapper )
      {
         wrapper( getInstance( uuid ), input_params, output_params, input.exception() );
         input.getResponse( output, output_params );

         invoke_success = true;
      }
      else
      {
         exception(
            input, output,
            NullInstance,
            "Null reference for \"" + input.callInfo().action + "\" action"
         );

         invoke_success = false;
      }
   }

   input.exception().popFrame();
   
   return invoke_success;
}

//------------------------------------------------------------------------------
bool RpcServerResource::destroy( RpcObject &input, RpcObject &output )
{
   input.exception().pushFrame( TraceFrame(
      "RpcServerResource",
      "destroy",
      __FILE__,
      __LINE__
   ));

   bool destroy_success = false;
   boost::uuids::uuid call_uuid = input.callInfo().uuid;

   InstanceMap::iterator instance_iterator = instances_.find( call_uuid );
   if ( instance_iterator != instances_.end() )
   {
      std::string input_params;

      input.getParams( input_params );

      InstanceWrapper* wrapper_ptr = instance_iterator->second;

      if ( wrapper_ptr == NULL )
      {
         exception(input, output, NullInstance, "Null wrapper reference");
         return false;
      }
      else
      {
         tugDtorHook(lpWrapper);
         wrapper_ptr->destroy( input_params );

         delete wrapper_ptr;
         instances_.erase( call_uuid );
         
         input.getResponse( output );
         mnInstanceCount--;

         destroy_success = true;
      }
   }
   else
   {
      exception( input, output, InvalidUIID, "Invalid instance UIID" );
   }

   input.exception().popFrame();
   
   return destroy_success;
}

//------------------------------------------------------------------------------
InstanceWrapper* RpcServerResource::getInstance( boost::uuids::uuid& uuid )
{
   InstanceWrapper* wrapper_ptr = NULL;
   InstanceMap::iterator instance_iterator = instances_.find( uuid );
   
   if ( instance_iterator != instances_.end() )
   {
      wrapper_ptr = instance_iterator->second;
   }
   
   return wrapper_ptr;
}

//------------------------------------------------------------------------------
bool RpcServerResource::addAction(
   const std::string&      action_name,
   InstanceWrapper::Method method
)
{
   bool add_action_success = false;
   
   if ( methods_.count( action_name ) != 0 )
   {
      MethodMap::iterator method_iterator = methods_.insert( std::make_pair(
         action_name, method
      ));

      add_action_success = method_iterator.second;
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
   request.exception().reporter = RpcException::Server;
   request.exception().id       = eid;
   request.exception().message  = message;

   request.getResponse( response );
}


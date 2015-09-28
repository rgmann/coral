#include "BlockingRpcSupervisor.h"
#include "Log.h"

using namespace liber;
using namespace liber::rpc;

//----------------------------------------------------------------------------
BlockingRpcSupervisor::BlockingRpcSupervisor() : RpcSupervisor()
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
      call_ptr->getResult( response_object_ );

      if ( response_object_.exception().id == NoException )
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
      exception_.reporter = RpcException::Client;
      exception_.id       = RpcCallTimeout;
      exception_.message  = "Time out elapsed waiting for resource response.";
   }

   call_ptr->dispose();
   exception_.popFrame();

   return ( exception_.id == NoException );
}



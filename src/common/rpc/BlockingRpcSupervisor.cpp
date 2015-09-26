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
   RpcClient&        rClient,
   const RpcObject&  request,
   PbMessage*        pResponse,
   int               nTimeoutMs)
{
   mException.pushFrame( TraceFrame(
      "BlockingRpcSupervisor",
      "invoke",
      __FILE__,
      __LINE__
   ));

   // Send the marshalled RPC to the RpcClient.
   RpcMarshalledCall* call_ptr = rClient.invokeRpc( request );

   if ( call_ptr->wait( nTimeoutMs ) )
   {
      call_ptr->getResult( mResponseObject );
      liber::log::status("BlockingRpcSupervisor::invoke: %s\n", mResponseObject.exception().toString().c_str());

      if ( mResponseObject.exception().id == NoException )
      {
         if ( pResponse )
         {
            mResponseObject.getParams(*pResponse);
         }
      }
      else
      {
         mException.pushTrace(mResponseObject.exception());
      }
   }
   else
   {
      mException.reporter = RpcException::Client;
      mException.id       = RpcCallTimeout;
      mException.message  = "Time out elapsed waiting for resource response.";
   }

   call_ptr->dispose();
   mException.popFrame();

   return ( mException.id == NoException );
}



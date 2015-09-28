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


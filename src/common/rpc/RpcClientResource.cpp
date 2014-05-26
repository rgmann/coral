#include "Log.h"
#include "RpcClientResource.h"
#include "BlockingRpcSupervisor.h"

using namespace liber;
using namespace liber::rpc;

//-----------------------------------------------------------------------------
RpcClientResource::RpcClientResource(RpcClient&         client,
                                     const std::string& classname)
: mrClient(client)
, mClassname(classname)
, mnTimeoutMs(3000)
{
   construct();
}

//-----------------------------------------------------------------------------
RpcClientResource::~RpcClientResource()
{
   destroy();
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
bool RpcClientResource::construct()
{
   bool lbSuccess = false;
   BlockingRpcSupervisor lSupervisor;
   RpcObject lRequestObject;
   
   marshallRequest(lRequestObject, "construct");
   
   lbSuccess = lSupervisor.invoke(mrClient,
                                  lRequestObject,
                                  NULL,
                                  mnTimeoutMs);

   if (lbSuccess)
   {
      mUiid = Md5Hash(lSupervisor.response().callInfo().uiid);
      log::debug("RpcClientResource::construct: success\n");
   }
   
   return lbSuccess;   
}

//-----------------------------------------------------------------------------
bool RpcClientResource::destroy()
{
   bool lbSuccess = false;
   BlockingRpcSupervisor lSupervisor;
   RpcObject lRequestObject;
   
   marshallRequest(lRequestObject, "destroy");

   lbSuccess = lSupervisor.invoke(mrClient,
                                  lRequestObject,
                                  NULL,
                                  mnTimeoutMs);

   if (lbSuccess)
   {
      mUiid.invalidate();
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::call(const std::string&  methodName,
                             const PbMessage&    request,
                             PbMessage&          response,
                             AsyncRpcSupervisor* pAsyncSupervisor)
{
   bool lbSuccess = false;
   RpcObject lRequestObject;

   BlockingRpcSupervisor lBlockingSupervisor;
   RpcSupervisor* lpSupervisor = &lBlockingSupervisor;

   mLastError.reset();
   mLastError.pushFrame(TraceFrame("RpcClientResource", "call",
                                   __FILE__, __LINE__));

   if (pAsyncSupervisor != NULL)
   {
     lpSupervisor = pAsyncSupervisor;
   }

   marshallRequest(lRequestObject, methodName, &request);
   
   lbSuccess = lpSupervisor->invoke(mrClient, lRequestObject, &response, mnTimeoutMs);

   if (!lbSuccess)
   {
     mLastError.pushTrace(lpSupervisor->exception());
   }

   mLastError.popFrame();
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
void RpcClientResource::
marshallRequest(RpcObject&         requestObject,
                const std::string& methodName,
                const PbMessage*   pRequestParameters)
{
  requestObject.callInfo().resource = mClassname;
  mUiid.get(&requestObject.callInfo().uiid);
  requestObject.callInfo().action = methodName;
  if (pRequestParameters) requestObject.setParams(*pRequestParameters);
}

//-----------------------------------------------------------------------------
bool RpcClientResource::invoke(const RpcObject& object,
                               RpcObject&       result,
                               ui32             nTimeoutMs)
{
   bool lbSuccess = false;

   // Send the marshalled RPC to the RpcClient.
   RpcMarshalledCall* lpCall = mrClient.invokeRpc(object);

   if ((lbSuccess = lpCall->wait(nTimeoutMs)) == true)
   {
      lpCall->getResult(result);
   }

   lpCall->dispose();

   return lbSuccess;
}


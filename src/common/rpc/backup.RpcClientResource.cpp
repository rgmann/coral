#include "Log.h"
#include "RpcClientResource.h"

using namespace liber;
using namespace liber::rpc;

//-----------------------------------------------------------------------------
RpcClientResource::RpcClientResource(RpcClient&         client,
                                     const std::string& classname)
: mrClient(client),
  mClassname(classname)
{
   construct(NULL);
}

//-----------------------------------------------------------------------------
RpcClientResource::~RpcClientResource()
{
   destroy(NULL);
}

//-----------------------------------------------------------------------------
RpcException RpcClientResource::getLastError()
{
   return mLastError;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::construct(const PbMessage* pParams)
{
   bool lbSuccess = false;
   RpcObject lInObject;
   RpcObject lOutObject;
   
   marshallRequest(lInObject, "construct", pParams);
   
   if ((lbSuccess = invoke(lInObject, lOutObject)) == true)
   {
      mUiid = Md5Hash(lOutObject.callInfo().uiid);
      log::debug("RpcClientResource::construct: success\n");
   }
   
   return lbSuccess;   
}

//-----------------------------------------------------------------------------
bool RpcClientResource::destroy(const PbMessage* pParams)
{
   bool lbSuccess = false;
   RpcObject lInObject;
   RpcObject lOutObject;
   
   marshallRequest(lInObject, "destroy", pParams);
      
   if ((lbSuccess = invoke(lInObject, lOutObject)) == true)
   {
      mUiid.invalidate();
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::call(const std::string& methodName,
                             const PbMessage&   request,
                             PbMessage&         response)
{
   bool lbSuccess = false;
   RpcObject lRequestObject;
   RpcObject lResponseObject;
   
   marshallRequest(lRequestObject, methodName, &request);
   
   if (invoke(lRequestObject, lResponseObject))
   {
      lbSuccess = (lResponseObject.exception().id == NoException);
      
      if (lbSuccess)
      {
         lResponseObject.getParams(response);
      }
      else
      {
         mLastError = lResponseObject.exception();
      }
   }
   
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

  if (pRequestParameters)
  {
    requestObject.setParams(*pRequestParameters);
  }
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


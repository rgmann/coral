#include <iostream>
#include "RpcClientResource.h"

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
   
   marshall(lInObject, "construct", pParams);
   
   if ((lbSuccess = invoke(lInObject, lOutObject)) == true)
   {
      mUiid = Md5Hash(lOutObject.callInfo().uiid);
   }
   
   return lbSuccess;   
}

//-----------------------------------------------------------------------------
bool RpcClientResource::destroy(const PbMessage* pParams)
{
   bool lbSuccess = false;
   RpcObject lInObject;
   RpcObject lOutObject;
   
   marshall(lInObject, "destroy", pParams);
      
   if ((lbSuccess = invoke(lInObject, lOutObject)) == true)
   {
      mUiid.invalidate();
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::call(const std::string& methodName,
                             const PbMessage&   params,
                             PbMessage&         returnValue)
{
   bool lbSuccess = false;
   RpcObject lInObject;
   RpcObject lOutObject;
   
   marshall(lInObject, methodName, &params);
   
   if (invoke(lInObject, lOutObject))
   {
      lbSuccess = (lOutObject.exception().id == NoException);
      
      if (lbSuccess)
      {
         lOutObject.getParams(returnValue);
      }
      else
      {
         mLastError = lOutObject.exception();
      }
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
void RpcClientResource::marshall(RpcObject&         object, 
                                 const std::string& action, 
                                 const PbMessage*   pParamList)
{
   object.callInfo().resource = mClassname;
   mUiid.get(&object.callInfo().uiid);
   object.callInfo().action   = action;

   if (pParamList)
   {
      object.setParams(*pParamList);
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


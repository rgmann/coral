#include <iostream>
#include "JsonTransportObject.h"
#include "RpcClientResource.h"

const Structure RpcClientResource::EmptyParamList("empty");

//-----------------------------------------------------------------------------
RpcClientResource::RpcClientResource(RpcClient         &client,
                                     const std::string &classname)
: mrClient(client),
  mClassname(classname),
  mnInstanceId(RpcClientResource::InvalidInstance)
{
   construct();
}

//-----------------------------------------------------------------------------
RpcClientResource::~RpcClientResource()
{
   destroy();
}

//-----------------------------------------------------------------------------
bool RpcClientResource::construct(const Structure &params)
{
   bool lbSuccess = false;
   RpcObject lObject;
   RpcObject lRetObject;
   
   marshall(lObject, "construct", params);
   
   if (lbSuccess = invoke(lObject, lRetObject))
   {
      RpcReturnValue lReturnValue;
      lRetObject.getReturnValue(lReturnValue);
      lReturnValue.get(mnInstanceId);
   }
   
   return lbSuccess;   
}

//-----------------------------------------------------------------------------
bool RpcClientResource::destroy(const Structure &params)
{
   bool lbSuccess = false;
   RpcObject lObject;
   RpcObject lRetObject;
   
   marshall(lObject, "destroy", params);
      
   if (lbSuccess = invoke(lObject, lRetObject))
   {
      mnInstanceId = -1;
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::call(const std::string  &methodName,
                             const Structure &params,
                             RpcReturnValue     &returnValue)
{
   bool lbSuccess = false;
   RpcObject lObject;
   RpcObject lRetObject;
   
   marshall(lObject, methodName, params);
   
   if (lbSuccess = invoke(lObject, lRetObject))
   {
      RpcReturnValue lReturnValue;
      lRetObject.getReturnValue(returnValue);
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::call(const std::string  &methodName,
                             RpcReturnValue     &returnValue)
{
   return call(methodName, EmptyParamList, returnValue);
}

//-----------------------------------------------------------------------------
void RpcClientResource::marshall(RpcObject &object, 
                                 const std::string &methodName, 
                                 const Structure &paramList)
{
   object.setClass(mClassname);
   object.setInstanceId(mnInstanceId);
   object.setMethod(methodName);
   object.setParams(paramList);
}

//-----------------------------------------------------------------------------
bool RpcClientResource::invoke(const RpcObject &object,
                               RpcObject       &result,
                               ui32            nTimeoutMs)
{
   bool lbSuccess = false;

   // Send the marshalled RPC to the RpcClient.
   RpcMarshalledCall* lpCall = mrClient.invokeRpc(object);

   if (lbSuccess = lpCall->wait(nTimeoutMs))
   {
      lpCall->getResult(result);
   }

   lpCall->dispose();

   return lbSuccess;
}


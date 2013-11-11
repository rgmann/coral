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
RpcError RpcClientResource::getLastError()
{
   return mLastError;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::construct(const Structure &params)
{
   bool lbSuccess = false;
   RpcObject lInObject;
   RpcObject lOutObject;
   
   marshall(lInObject, "construct", params);
   
   if ((lbSuccess = invoke(lInObject, lOutObject)) == true)
   {
      Structure lReturnValue;
      lOutObject.getParams(lReturnValue);
      lReturnValue.get(RpcReturnValue, mnInstanceId);
   }
   
   return lbSuccess;   
}

//-----------------------------------------------------------------------------
bool RpcClientResource::destroy(const Structure &params)
{
   bool lbSuccess = false;
   RpcObject lInObject;
   RpcObject lOutObject;
   
   marshall(lInObject, "destroy", params);
      
   if ((lbSuccess = invoke(lInObject, lOutObject)) == true)
   {
      mnInstanceId = -1;
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::call(const std::string  &methodName,
                             const Structure    &params,
                             Structure          &returnValue)
{
   bool lbSuccess = false;
   RpcObject lInObject;
   RpcObject lOutObject;
   
   marshall(lInObject, methodName, params);
   
   if (invoke(lInObject, lOutObject))
   {
      lbSuccess = (lOutObject.getException() == NoException);
      
      if (lbSuccess)
      {
         lOutObject.getParams(returnValue);
      }
      else
      {
         mLastError = lOutObject.getError();
      }
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RpcClientResource::call(const std::string  &methodName,
                             Structure          &returnValue)
{
   return call(methodName, EmptyParamList, returnValue);
}

//-----------------------------------------------------------------------------
void RpcClientResource::marshall(RpcObject         &object, 
                                 const std::string &methodName, 
                                 const Structure   &paramList)
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

   if ((lbSuccess = lpCall->wait(nTimeoutMs)) == true)
   {
      lpCall->getResult(result);
   }

   lpCall->dispose();

   return lbSuccess;
}


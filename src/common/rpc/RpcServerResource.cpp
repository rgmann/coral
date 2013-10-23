#include <iostream>
#include "RpcServerResource.h"

//------------------------------------------------------------------------------
RpcServerResource::RpcServerResource(const std::string &name)
:  mName(name),
   mnCurrentInstId(0)
{
}

//------------------------------------------------------------------------------
std::string RpcServerResource::getName() const
{
   return mName;
}

//------------------------------------------------------------------------------
bool RpcServerResource::unmarshall(const RpcObject &input, RpcObject &output)
{
   bool lbSuccess = true;
   
   int lInstanceId = input.getInstanceId();
   
   if (!input.isValid())
   {
      exception(MissingParameters, input, output);
      return false;
   }
   
   if (input.methodEquals("construct"))
   {
      lbSuccess = construct(input, output);
   }
   else if (input.methodEquals("destroy"))
   {
      lbSuccess = destroy(input, output);
   }
   else
   {
      if (isValidInstance(lInstanceId))
      {
         lbSuccess = invoke(lInstanceId, input, output);
      }
      else
      {
         exception(InvalidInstanceId, input, output);
         lbSuccess = false;
      }
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool RpcServerResource::construct(const RpcObject &input, RpcObject &output)
{
   bool lbSuccess = false;
   Structure lParamList;
   
   int lInstanceId = getNextInstanceId();
   
   if (mInstances.count(lInstanceId) != 0)
   {
      exception(InstIdAssignmentErr, input, output);
      return false;
   }
   
   input.getParams(lParamList);
   mInstances[lInstanceId] = createInstance();
   
   lbSuccess = mInstances[lInstanceId]->initialize(lParamList);
   
   input.getResponse(output, RpcReturnValue(lInstanceId));
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool RpcServerResource::invoke(int instId,
                               const RpcObject &input,
                               RpcObject &output)
{
   bool lbSuccess = (mMethodMap.count(input.getMethod()) != 0);
   
   if (lbSuccess)
   {
      Structure lParamList;
      RpcReturnValue lRetVal;

      try
      {
         InstanceWrapper::Method wrapper;
         input.getParams(lParamList);
         wrapper = mMethodMap[input.getMethod()];
         wrapper(getInstance(instId), lParamList, lRetVal);
         input.getResponse(output, lRetVal);
      }
      catch (RpcException e)
      {
         exception(e, input, output);
         lbSuccess = false;
      }
   }
   else
   {
      exception(UnknownMethod, input, output);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool RpcServerResource::destroy(const RpcObject &input, RpcObject &output)
{
   if (mInstances.count(input.getInstanceId()) == 0)
   {
      exception(InvalidInstanceId, input, output);
      return false;
   }
   
   mInstances[input.getInstanceId()]->destroy(input);
   delete mInstances[input.getInstanceId()];
   mInstances.erase(input.getInstanceId());
   
   input.getResponse(output);
   
   return true;
}

//------------------------------------------------------------------------------
InstanceWrapper* RpcServerResource::getInstance(int instId)
{
   InstanceWrapper* lpInstance = NULL;
   
   if (mInstances.count(instId) != 0)
   {
      lpInstance = mInstances[instId];
   }
   
   return lpInstance;
}

//------------------------------------------------------------------------------
bool RpcServerResource::addAction(const std::string &actionName,
                                  InstanceWrapper::Method method)
{
   bool lbMethodExists = (mMethodMap.count(actionName) != 0);
   
   if (!lbMethodExists)
   {
      mMethodMap[actionName] = method;
   }
   
   return lbMethodExists;
}

//------------------------------------------------------------------------------
void RpcServerResource::exception(RpcException exception,
                                  const RpcObject &input,
                                  RpcObject &output)
{
   input.getResponse(output, exception);
}

//------------------------------------------------------------------------------
int RpcServerResource::getNextInstanceId()
{
   return ++mnCurrentInstId;
}

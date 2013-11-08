#include <utility>
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
   mInstances.insert(std::make_pair(lInstanceId, createInstance()));
   
   lbSuccess = mInstances.find(lInstanceId)->second->initialize(lParamList);
   
   Structure lReturnParams;
   lReturnParams.set(RpcReturnValue, lInstanceId);
   input.getResponse(output, lReturnParams);
   
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
      Structure lInParams;
      Structure lOutParams;

      InstanceWrapper::Method wrapper = NULL;
      input.getParams(lInParams);

      wrapper = mMethodMap.find(input.getMethod())->second;
      if (wrapper)
      {
         wrapper(getInstance(instId), lInParams, lOutParams);
         input.getResponse(output, lOutParams);
      }
      else
      {
         exception(NullInstance, input, output);
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
   
   mInstances.find(input.getInstanceId())->second->destroy(input);
   delete mInstances.find(input.getInstanceId())->second;
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
      lpInstance = mInstances.find(instId)->second;
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
      mMethodMap.insert(std::make_pair(actionName, method));
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

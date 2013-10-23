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
   
   if (!RpcObject::IsValidObject(input))
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
         lbSuccess = invoke(input, output);
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

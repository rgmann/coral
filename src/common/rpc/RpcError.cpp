#include "RpcError.h"

static const std::string ExceptionType("etype");
static const std::string ResourceName("rname");
static const std::string InstanceId("iid");
static const std::string ActionName("aname");
static const std::string RpcId("rid");
static const std::string ErrorMessage("edesc");
static const std::string ErrorReporter("erep");

//-----------------------------------------------------------------------------
RpcError::RpcError()
: exceptionId(NoException),
  instanceId(-1),
  rpcId(-1),
  reporter(RpcError::Unknown)
{
}

//-----------------------------------------------------------------------------
std::string RpcError::toString() const
{
   std::string lErrorString = "";

   lErrorString += resourceName + "(";
   switch (reporter)
   {
   case Server: lErrorString += "Server)"; break;
   case Client: lErrorString += "Client)"; break;
   default: lErrorString += "Unknown)"; break;
   }

   lErrorString += "::" + actionName + ": ";
   lErrorString += ToRpcExceptionString(exceptionId);

   if (message.length() > 0)
   {
      lErrorString += " > " + message;
   }

   return lErrorString;
}

//-----------------------------------------------------------------------------
Structure RpcError::toStructure() const
{
   Structure lErrorStruct("RpcError");

   lErrorStruct.set(ExceptionType, (i32)exceptionId);
   lErrorStruct.set(ResourceName, resourceName);
   lErrorStruct.set(InstanceId, instanceId);
   lErrorStruct.set(ActionName, actionName);
   lErrorStruct.set(RpcId, rpcId);
   lErrorStruct.set(ErrorMessage, message);
   lErrorStruct.set(ErrorReporter, (i32)reporter);

   return lErrorStruct;
}

//-----------------------------------------------------------------------------
bool RpcError::fromStructure(const Structure& error)
{
   bool lbSuccess = true;

   i32 lExceptionId = -1;
   lbSuccess &= error.get(ExceptionType, lExceptionId);
   exceptionId = (RpcException)lExceptionId;

   lbSuccess &= error.get(ResourceName, resourceName);
   lbSuccess &= error.get(InstanceId, instanceId);
   lbSuccess &= error.get(ActionName, actionName);
   lbSuccess &= error.get(RpcId, rpcId);
   lbSuccess &= error.get(ErrorMessage, message);

   i32 lReporterType = Unknown;
   lbSuccess &= error.get(ErrorReporter, lReporterType);
   reporter = (Reporter)lReporterType;

   return lbSuccess;
}


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
{
}

//-----------------------------------------------------------------------------
RpcError::~RpcError()
{
}

//-----------------------------------------------------------------------------
void RpcError::exception(RpcException etype)
{
   set(ExceptionType, static_cast<i32>(etype));
}

//-----------------------------------------------------------------------------
RpcException RpcError::exception() const
{
   i32 lExceptionType = NoException;
   get(ExceptionType, lExceptionType);
   return static_cast<RpcException>(lExceptionType);
}

//-----------------------------------------------------------------------------
void RpcError::resource(const std::string& resourceName)
{
   set(ResourceName, resourceName);
}

//-----------------------------------------------------------------------------
std::string RpcError::resource() const
{
   std::string lResourceName = "nil";
   get(ResourceName, lResourceName);
   return lResourceName;
}

//-----------------------------------------------------------------------------
void RpcError::instance(i32 iid)
{
   set(InstanceId, iid);
}

//-----------------------------------------------------------------------------
i32 RpcError::instance() const
{
   i32 lIid = -1;
   get(InstanceId, lIid);
   return lIid;
}

//-----------------------------------------------------------------------------
void RpcError::action(const std::string& actionName)
{
   set(ActionName, actionName);
}

//-----------------------------------------------------------------------------
std::string RpcError::action() const
{
   std::string lActionName = "nil";
   get(ActionName, lActionName);
   return lActionName;
}

//-----------------------------------------------------------------------------
void RpcError::rpcId(i32 rid)
{
   set(RpcId, rid);
}

//-----------------------------------------------------------------------------
i32 RpcError::rpcId() const
{
   i32 lRpcId = -1;
   get(RpcId, lRpcId);
   return lRpcId;
}

//-----------------------------------------------------------------------------
void RpcError::message(const std::string& errorMessage)
{
   set(ErrorMessage, errorMessage);
}

//-----------------------------------------------------------------------------
std::string RpcError::message() const
{
   std::string lMessage = "nil";
   get(ErrorMessage, lMessage);
   return lMessage;
}

//-----------------------------------------------------------------------------
void RpcError::reporter(Reporter errorReporter)
{
   set(ErrorReporter, static_cast<i32>(errorReporter));
}

//-----------------------------------------------------------------------------
RpcError::Reporter RpcError::reporter() const
{
   i32 lErrorReporter = Unknown;
   get(ErrorReporter, lErrorReporter);
   return static_cast<Reporter>(lErrorReporter);
}

//-----------------------------------------------------------------------------
std::string RpcError::toString() const
{
   std::string lErrorString = "";

   lErrorString += resource() + "(";
   switch (reporter())
   {
   case Server: lErrorString += "Server)"; break;
   case Client: lErrorString += "Client)"; break;
   default: lErrorString += "Unknown)"; break;
   }

   lErrorString += "::" + action() + ": ";
   lErrorString += ToRpcExceptionString(exception()) + " > ";
   lErrorString += message();

   return lErrorString;
}


#include "RpcObject.h"

static const std::string ClassNameField("class");
static const std::string MethodNameField("method");
static const std::string InstIdField("iid");
static const std::string RpcIdFieldName("rpcid");
static const std::string ExceptionFieldName("eid");
static const std::string RetvalNameField("retval");
static const std::string ParameterField("params");

//-----------------------------------------------------------------------------
RpcObject::RpcObject()
:  Structure("rpcobject")
{
}

//-----------------------------------------------------------------------------
RpcObject::RpcObject(const std::string &className, 
                     const std::string &methodName)
:  Structure("rpcobject")
{
   setClass(className);
   setMethod(methodName);
}

//-----------------------------------------------------------------------------
RpcObject::~RpcObject()
{
}

//-----------------------------------------------------------------------------
void RpcObject::setClass(const std::string &className)
{
   set(ClassNameField, className);
}

//-----------------------------------------------------------------------------
std::string RpcObject::getClass() const
{
   std::string lClassname = "";
   get(ClassNameField, lClassname);
   return lClassname;
}

//-----------------------------------------------------------------------------
void RpcObject::setMethod(const std::string &methodName)
{
   set(MethodNameField, methodName);
}

//-----------------------------------------------------------------------------
std::string RpcObject::getMethod() const
{
   std::string lMethodname = "";
   get(MethodNameField, lMethodname);
   return lMethodname;
}

//-----------------------------------------------------------------------------
bool RpcObject::methodEquals(const std::string &methodName) const
{
   return (getMethod().compare(methodName) == 0);
}

//-----------------------------------------------------------------------------
void RpcObject::setInstanceId(int nId)
{
   set(InstIdField, (i32)nId);
}

//-----------------------------------------------------------------------------
int RpcObject::getInstanceId() const
{
   i32 lInstanceId = -1;
   
   if (hasField(InstIdField))
   {
      get(InstIdField, lInstanceId);
   }
   
   return (int)lInstanceId;
}

//-----------------------------------------------------------------------------
bool RpcObject::isValid() const
{
   return hasField(ClassNameField) && hasField(MethodNameField);
}

//-----------------------------------------------------------------------------
void RpcObject::setRpcId(i64 rpcId)
{
   set(RpcIdFieldName, rpcId);
}

//-----------------------------------------------------------------------------
i64 RpcObject::getRpcId() const
{
   i64 lRpcId = -1;
   
   if (hasField(RpcIdFieldName))
   {
      get(RpcIdFieldName, lRpcId);
   }
   
   return lRpcId;
}

//-----------------------------------------------------------------------------
void RpcObject::
setException(RpcException exception, const std::string& message)
{
   RpcError lError;

   lError.exceptionId = exception;
   lError.message     = message;

   setError(lError);
}

//-----------------------------------------------------------------------------
void RpcObject::setError(RpcError& error)
{
   error.resourceName = getClass();
   error.actionName   = getMethod();
   error.instanceId   = getInstanceId();
   error.rpcId        = getRpcId();
   set(ExceptionFieldName, error.toStructure());
}

//-----------------------------------------------------------------------------
RpcException RpcObject::getException() const
{
   return getError().exceptionId;
}

//-----------------------------------------------------------------------------
RpcError RpcObject::getError() const
{
   RpcError lError;
   Structure lErrorStruct;

   if (get(ExceptionFieldName, lErrorStruct))
   {
      lError.fromStructure(lErrorStruct);
   }

   return lError;
}

//-----------------------------------------------------------------------------
void RpcObject::setParams(const Structure &value)
{
   set(ParameterField, value);
}

//-----------------------------------------------------------------------------
bool RpcObject::getParams(Structure &value) const
{
   return get(ParameterField, value);
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse(RpcObject &response) const
{
   if (!isValid()) return false;
   
   response.setClass(getClass());
   response.setMethod(getMethod());
   response.setRpcId(getRpcId());
   response.setInstanceId(getInstanceId());
   
   return true;
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse(RpcObject &response,
                            const Structure &value) const
{
   if (!getResponse(response)) return false;
   response.setParams(value);
   return true;
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse(RpcObject &response, RpcError& error) const
{
   if (!getResponse(response)) return false;
   response.setError(error);
   return true;
}

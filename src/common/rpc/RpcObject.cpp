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
   Structure lException;

   lException.set("e", exception);
   lException.set("m", message);

   set(ExceptionFieldName, lException);
}

//-----------------------------------------------------------------------------
RpcException RpcObject::getException() const
{
   i32 lnExceptionId = NoException;
   
   if (hasField(ExceptionFieldName))
   {
      Structure lException;
      get(ExceptionFieldName, lException);
      lException.get("e", lnExceptionId);
   }
   
   return static_cast<RpcException>(lnExceptionId);
}

//-----------------------------------------------------------------------------
RpcError RpcObject::getError() const
{
   RpcError lError;

   if (hasField(ExceptionFieldName))
   {
      Structure lException;
      get(ExceptionFieldName, lException);

      i32 lnEType = NoException;
      lException.get("e", lnEType);
      lError.exception(static_cast<RpcException>(lnEType));

      std::string lErrorMsg = "";
      if (lException.get("m", lErrorMsg))
      {
         lError.message(lErrorMsg);
      }

      lError.resource(getClass());
      lError.action(getMethod());
      lError.instance(getInstanceId());
      lError.rpcId(getRpcId());
   }

   return lError;
}

//-----------------------------------------------------------------------------
/*void RpcObject::setReturnValue(const RpcReturnValue &value)
{
   set(RetvalNameField, value);
}*/

//-----------------------------------------------------------------------------
/*bool RpcObject::getReturnValue(RpcReturnValue &value) const
{
   return get(RetvalNameField, value);
}*/

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
bool RpcObject::getResponse(RpcObject &response, RpcException exception) const
{
   if (!getResponse(response)) return false;
   response.setException(exception);
   return true;
}

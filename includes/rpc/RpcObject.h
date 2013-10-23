#ifndef RPC_OBJECT_H
#define RPC_OBJECT_H

#include "Structure.h"
#include "RpcError.h"
#include "RpcReturnValue.h"

class RpcObject : public Structure {
public:

   RpcObject();
   RpcObject(const std::string &className, const std::string &methodName);
   virtual ~RpcObject();

   void setClass(const std::string &className);
   std::string getClass() const;

   void setMethod(const std::string &methodName);
   std::string getMethod() const;
   
   void setInstanceId(int nId);
   int getInstanceId() const;

   bool methodEquals(const std::string &methodName) const;
   
   void setRpcId(i64 rpcId);
   i64  getRpcId() const;
   
   void setParams(const Structure &paramList);
   bool getParams(Structure &paramList) const;
   
   virtual bool isValid() const;
      
   void setException(RpcException exception, const std::string& message = "");
   RpcException getException() const;
   RpcError getError() const;
   
   void setReturnValue(const RpcReturnValue &value);
   bool getReturnValue(RpcReturnValue &value) const;
   
   bool getResponse(RpcObject &response) const;
   bool getResponse(RpcObject &response, const RpcReturnValue &value) const;
   bool getResponse(RpcObject &response, RpcException) const;
};

#endif // RPC_OBJECT_H

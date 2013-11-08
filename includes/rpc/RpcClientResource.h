#ifndef RPC_CLIENT_RESOURCE_H
#define RPC_CLIENT_RESOURCE_H

#include <string>
#include "RpcObject.h"
#include "RpcError.h"
#include "RpcReturnValue.h"
#include "RpcClient.h"

class RpcClientResource
{
public:

   RpcClientResource(RpcClient &client, const std::string &classname);
   virtual ~RpcClientResource();
   
   enum { InvalidInstance = -1 };
   
   RpcError getLastError();

protected:
   
   static const Structure EmptyParamList;
   
   void setCallTimeout(ui32 nTimeoutMs);
   
   bool construct(const Structure &paramList = EmptyParamList);
   
   bool destroy(const Structure &paramList = EmptyParamList);
   
   bool call(const std::string &methodName,
             Structure         &returnValue);
   
   bool call(const std::string &methodName,
             const Structure   &paramList,
             Structure         &returnValue);

   void marshall(RpcObject         &object, 
                 const std::string &methodName, 
                 const Structure   &paramList);
   
private:
   
   bool invoke(const RpcObject &object,
               RpcObject       &result,
               ui32            nTimeoutMs = 3000);

protected:

   RpcClient &mrClient;
   
   std::string mClassname;
   
   int mnInstanceId;

   RpcError mLastError;
};

#endif // RPC_CLIENT_RESOURCE_H

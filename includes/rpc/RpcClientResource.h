#ifndef RPC_CLIENT_RESOURCE_H
#define RPC_CLIENT_RESOURCE_H

#include <string>
#include "RpcObject.h"
#include "RpcException.h"
#include "RpcClient.h"
#include "Md5Hash.h"

namespace liber {
namespace rpc {

class RpcClientResource {
public:

   RpcClientResource(RpcClient &client, const std::string &classname);
   virtual ~RpcClientResource();
   
   enum { InvalidInstance = -1 };
   
   RpcException getLastError();

protected:
   
   void setCallTimeout(ui32 nTimeoutMs);
   
   bool construct(const PbMessage* pParams = NULL);
   
   bool destroy(const PbMessage* pParams = NULL);
   
   bool call(const std::string &methodName,
             const PbMessage   &paramList,
             PbMessage         &returnValue);

   void marshall(RpcObject&         object, 
                 const std::string& methodName, 
                 const PbMessage*   pParamList);
   
private:
   
   bool invoke(const RpcObject& object,
               RpcObject&       result,
               ui32             nTimeoutMs = 3000);

protected:

   RpcClient &mrClient;
   
   std::string mClassname;
   
   Md5Hash mUiid;

   RpcException mLastError;
};

}}

#endif // RPC_CLIENT_RESOURCE_H

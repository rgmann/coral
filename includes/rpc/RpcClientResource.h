#ifndef RPC_CLIENT_RESOURCE_H
#define RPC_CLIENT_RESOURCE_H

#include <string>
#include "RpcObject.h"
#include "RpcException.h"
#include "RpcClient.h"
#include "AsyncRpcSupervisor.h"
#include "Md5Hash.h"


namespace liber {
namespace rpc {

class RpcClientResource {
public:

   RpcClientResource(RpcClient &client, const std::string &classname);
   virtual ~RpcClientResource();
   
   enum { InvalidInstance = -1 };

   void setTimeout(int nTimeoutMs);
 
   RpcException getLastError();

protected:
   
   void setCallTimeout(ui32 nTimeoutMs);
   
   bool construct();
   
   bool destroy();
   
   bool call(const std::string&  methodName,
             const PbMessage&    request,
             PbMessage&          response,
             AsyncRpcSupervisor* pSupervisor = NULL);

   void marshallRequest(RpcObject&         requestObject,
                        const std::string& methodName,
                        const PbMessage*   pReqeustParameters = NULL);
   
private:
   
   bool invoke(const RpcObject& object,
               RpcObject&       result,
               ui32             nTimeoutMs);

protected:

   RpcClient &mrClient;
   
   std::string mClassname;
   
   Md5Hash mUiid;

   RpcException mLastError;

   int mnTimeoutMs;
};

}}

#endif // RPC_CLIENT_RESOURCE_H

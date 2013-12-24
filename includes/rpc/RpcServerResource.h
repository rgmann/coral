#ifndef RPC_SERVER_RESOURCE_H
#define RPC_SERVER_RESOURCE_H

#include <map>
#include <string>
#include "Queue.h"
#include "Mutex.h"
#include "RpcObject.h"
#include "InstanceWrapper.h"
#include "Md5Hash.h"

namespace liber {
namespace rpc {

class RpcServerResource {
public:
   
   RpcServerResource(const std::string &name);

   bool unmarshall(RpcObject &input, RpcObject &output);
   
   std::string getName() const;

   virtual void registerActions() = 0;

protected:

   virtual bool isValidInstance(Md5Hash& uiid);
   
   bool construct(RpcObject &input, RpcObject &output);
   
   bool destroy(RpcObject &input, RpcObject &output);
   
   bool invoke(Md5Hash& instId, RpcObject &input, RpcObject &output);
      
   void exception(RpcObject&         input,
                  RpcObject&         output,
                  RpcErrorId         eid,
                  const std::string& message = "");
   
   void createUIID(Md5Hash& hash);

   virtual InstanceWrapper* createInstance() = 0;
   InstanceWrapper* getInstance(Md5Hash& uiid);
      
   bool addAction(const std::string &name,
                  InstanceWrapper::Method wrapper);

   static const i32 HookRegTimeoutMs = 500;  
   virtual bool tugCtorHook(InstanceWrapper* pInst) = 0;
   virtual bool tugDtorHook(InstanceWrapper* pInst) = 0;

protected:
 
   Mutex mHookMutex;

private:
   
   std::string mName;
   
   std::map<Md5Hash, InstanceWrapper*> mInstances;
   
   std::map<std::string, InstanceWrapper::Method> mMethodMap;
   
   int mnCurrentInstId;
   int mnInstanceCount;
};

}}

#endif // RPC_SERVER_RESOURCE_H

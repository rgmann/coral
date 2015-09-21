#ifndef RPC_SERVER_RESOURCE_H
#define RPC_SERVER_RESOURCE_H

#include <map>
#include <string>
#include <boost/uuid/uuid.hpp>
#include "Queue.h"
#include <boost/thread/mutex.hpp>
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

   virtual bool isValidInstance(boost::uuids::uuid& uuid);
   
   bool construct(RpcObject &input, RpcObject &output);
   
   bool destroy(RpcObject &input, RpcObject &output);
   
   bool invoke(boost::uuids::uuid& uuid, RpcObject &input, RpcObject &output);
      
   void exception(RpcObject&         input,
                  RpcObject&         output,
                  RpcErrorId         eid,
                  const std::string& message = "");
   

   virtual InstanceWrapper* createInstance() = 0;
   InstanceWrapper* getInstance(boost::uuids::uuid& uuid);
      
   bool addAction(const std::string &name,
                  InstanceWrapper::Method wrapper);

   static const i32 HookRegTimeoutMs = 500;  
   virtual bool tugCtorHook(InstanceWrapper* pInst) = 0;
   virtual bool tugDtorHook(InstanceWrapper* pInst) = 0;

protected:
 
   boost::mutex mHookMutex;

private:
   
   std::string resource_name_;
   
   typedef  std::map<boost::uuids::uuid,InstanceWrapper*> InstanceMap;
   InstanceMap instances_;

   typedef  std::map<std::string,InstanceWrapper::Method> MethodMap;
   MethodMap methods_;
   
   int mnInstanceCount;
};

}}

#endif // RPC_SERVER_RESOURCE_H

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

class RpcServiceAction {
public:

   virtual void operator() ( const std::string& request, std::string& response, RpcException& e ) = 0;
};

class RpcServerResource {
public:
   
   RpcServerResource(const std::string &name);

   bool unmarshall(RpcObject &input, RpcObject &output);
   
   std::string getName() const;

   virtual void registerActions() = 0;

protected:
   
   bool invoke(boost::uuids::uuid& uuid, RpcObject &input, RpcObject &output);
      
   void exception(RpcObject&         input,
                  RpcObject&         output,
                  RpcErrorId         eid,
                  const std::string& message = "");
      
   bool addAction(const std::string &name,
                  RpcServiceAction* action_ptr );


protected:
 
   boost::mutex mHookMutex;

private:
   
   std::string resource_name_;

   typedef  std::map<std::string,RpcServiceAction*> ActionMap;
   ActionMap actions_;
   
};

}}

#endif // RPC_SERVER_RESOURCE_H

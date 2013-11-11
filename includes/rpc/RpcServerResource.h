#ifndef RPC_SERVER_RESOURCE_H
#define RPC_SERVER_RESOURCE_H

#include <string>
#include "Queue.h"
#include "RpcObject.h"
#include "InstanceWrapper.h"

class RpcServerResource
{
public:
   
   RpcServerResource(const std::string &name);

   bool unmarshall(const RpcObject &input, RpcObject &output);
   
   std::string getName() const;
   
   virtual void registerActions() = 0;
   
protected:
   
   virtual bool isValidInstance(int nInstId) = 0;
   
   bool construct(const RpcObject &input, RpcObject &output);
   
   bool destroy(const RpcObject &input, RpcObject &output);
   
   bool invoke(int instId, const RpcObject &input, RpcObject &output);
      
   void exception(const RpcObject&   input,
                  RpcObject&         output,
                  RpcException       exception,
                  const std::string& message = "");
   
   int getNextInstanceId();
   
   virtual InstanceWrapper* createInstance() = 0;
   InstanceWrapper* getInstance(int instId);
      
   bool addAction(const std::string &name,
                  InstanceWrapper::Method wrapper);
   
private:
   
   std::string mName;
   
   std::map<int, InstanceWrapper*> mInstances;
   
   std::map<std::string, InstanceWrapper::Method> mMethodMap;
   
   int mnCurrentInstId;
};

#endif // RPC_SERVER_RESOURCE_H

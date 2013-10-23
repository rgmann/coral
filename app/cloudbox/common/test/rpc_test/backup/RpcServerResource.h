#ifndef RPC_SERVER_RESOURCE_H
#define RPC_SERVER_RESOURCE_H

#include <string>
#include "Queue.h"
#include "RpcObject.h"

class RpcServerResource
{
public:
   
   RpcServerResource(const std::string &name);

   bool unmarshall(const RpcObject &input, RpcObject &output);
   
   std::string getName() const;
   
protected:
   
   virtual bool isValidInstance(int nInstId) = 0;
   
   virtual bool construct(const RpcObject &input, RpcObject &output) = 0;
   
   virtual bool destroy(const RpcObject &input, RpcObject &output) = 0;
   
   virtual bool invoke(const RpcObject &input, RpcObject &output) = 0;
      
   void exception(RpcException exception,
                  const RpcObject& input,
                  RpcObject &object);
   
   int getNextInstanceId();
   
private:
   
   std::string mName;
      
   int mnCurrentInstId;
};

#endif // RPC_SERVER_RESOURCE_H

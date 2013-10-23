#ifndef RPC_ERROR_H
#define RPC_ERROR_H

#include <string>
#include "RpcCommon.h"
#include "Structure.h"

class RpcError : public Structure
{
public:

   RpcError();
   ~RpcError();

   void exception(RpcException etype);
   RpcException exception() const;

   void resource(const std::string& resourceName);
   std::string resource() const;

   void instance(i32 iid);
   i32 instance() const;

   void action(const std::string& actionName);
   std::string action() const;

   void rpcId(i32 rid);
   i32 rpcId() const;

   void message(const std::string& errorMessage);
   std::string message() const;

   enum Reporter
   {
      Unknown,
      Server,
      Client
   };
   void reporter(Reporter errorReporter);
   Reporter reporter() const;

   std::string toString() const;
};

#endif // RPC_ERROR_H

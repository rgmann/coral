#ifndef RPC_ERROR_H
#define RPC_ERROR_H

#include <string>
#include "RpcCommon.h"
#include "Structure.h"

class RpcError
{
public:

   RpcError();

   RpcException exceptionId;

   std::string resourceName;

   i32 instanceId;

   std::string actionName;

   i32 rpcId;

   std::string message;

   enum Reporter
   {
      Unknown,
      Server,
      Client
   };
   Reporter reporter;

   std::string toString() const;

   Structure toStructure() const;
   bool fromStructure(const Structure& error);

   bool valid() const;

};

#endif // RPC_ERROR_H

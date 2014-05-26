#ifndef RPC_COMMON_H
#define RPC_COMMON_H

#include <string>
#include "BaseTypes.h"
#include "CryptoHashTypes.h"

namespace liber {
namespace rpc {

enum RpcErrorId
{
   NoException = 0,
   
   // Client exceptions
   RpcCallTimeout,
   RpcCallCancelled,

   // Server Exceptions
   UnknownResource,
   MissingUIID,
   UIIDAssignmentErr,
   InvalidUIID,
   MissingParameters,

   // Instance ID was found byte the pointer is NULL
   NullInstance,
   UnknownMethod,

   // Developer has not filled in an action's implementation.
   UnimplementedAction
};

inline std::string ToRpcErrorString(RpcErrorId error)
{
   std::string lsException;
   
   switch (error) {
      case NoException: lsException = "NoException"; break;

      case RpcCallTimeout:   return "RpcCallTimeout";
      case RpcCallCancelled: return "RpcCallCancelled";

      case UnknownResource: lsException = "UnknownResource"; break;
      case MissingUIID: lsException = "MissingUIID"; break;
      case UIIDAssignmentErr: lsException = "UIIDAssignmentErr"; break;
      case InvalidUIID: lsException = "InvalidUIID"; break;
      case MissingParameters: lsException = "MissingParameters"; break;
      case NullInstance: lsException = "NullInstance"; break;
      case UnknownMethod: lsException = "UnknownMethod"; break;
      case UnimplementedAction: lsException = "UnimplementedAction"; break;
      default: lsException = "Unknown"; break;
   }
   
   return lsException;
};


struct RpcCallInfo {
   std::string resource;
   std::string action;
   Hash128     uiid; // Unique Instance ID
   i64         rpcId;
};

}}

#endif // RPC_COMMON_H

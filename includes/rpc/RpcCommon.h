#ifndef RPC_COMMON_H
#define RPC_COMMON_H

#include <string>

enum RpcException
{
   NoException = 0,
   
   // Client exceptions
   RpcCallTimeout,

   // Server Exceptions
   InvalidClassname,
   MissingInstanceId,
   InstIdAssignmentErr,
   InvalidInstanceId,
   MissingParameters,
   UnknownMethod
};

inline std::string ToRpcExceptionString(RpcException exception)
{
   std::string lsException;
   
   switch (exception) {
      case NoException: lsException = "NoException"; break;
      case InvalidClassname: lsException = "InvalidClassname"; break;
      case MissingInstanceId: lsException = "MissingInstanceId"; break;
      case InvalidInstanceId: lsException = "InvalidInstanceId"; break;
      case MissingParameters: lsException = "MissingParameters"; break;
      case UnknownMethod: lsException = "UnknownMethod"; break;
      default: lsException = "Unknown"; break;
   }
   
   return lsException;
};

#endif // RPC_COMMON_H

// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#ifndef RPC_COMMON_H
#define RPC_COMMON_H

#include <string>
#include <boost/uuid/uuid.hpp>
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
   NullAction,
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
      case NullAction: lsException = "NullAction"; break;
      case UnknownMethod: lsException = "UnknownMethod"; break;
      case UnimplementedAction: lsException = "UnimplementedAction"; break;
      default: lsException = "Unknown"; break;
   }
   
   return lsException;
};


struct RpcCallInfo {
   std::string resource;
   std::string action;
   boost::uuids::uuid uuid; // Unique Instance ID
};

}}

#endif // RPC_COMMON_H

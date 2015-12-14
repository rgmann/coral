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

namespace coral {
namespace rpc {

enum RpcErrorId
{
   kNoException = 0,
   
   // Client exceptions
   kRpcCallTimeout,
   kRpcCallCancelled,

   // Server Exceptions
   kUnknownResource,
   kMissingUIID,
   kUIIDAssignmentErr,
   kInvalidUIID,
   kMissingParameters,

   // Instance ID was found byte the pointer is NULL
   kNullAction,
   kUnknownMethod,

   // Developer has not filled in an action's implementation.
   kUnimplementedAction
};

inline std::string ToRpcErrorString( RpcErrorId error )
{
   std::string error_message( "Unknown" );
   
   switch (error) {
      case kNoException:       error_message = "NoException"; break;
      case kRpcCallTimeout:    error_message = "RpcCallTimeout"; break;
      case kRpcCallCancelled:  error_message = "RpcCallCancelled"; break;
      case kUnknownResource:   error_message = "UnknownResource"; break;
      case kMissingUIID:       error_message = "MissingUIID"; break;
      case kUIIDAssignmentErr: error_message = "UIIDAssignmentErr"; break;
      case kInvalidUIID:       error_message = "InvalidUIID"; break;
      case kMissingParameters: error_message = "MissingParameters"; break;
      case kNullAction:        error_message = "NullAction"; break;
      case kUnknownMethod:     error_message = "UnknownMethod"; break;
      case kUnimplementedAction: error_message = "UnimplementedAction"; break;
      default: break;
   }
   
   return error_message;
};


///
/// Call descriptor
///
struct RpcCallInfo {

   // Resource name
   std::string resource;

   // Procedure name
   std::string action;

   // Unique invocation ID
   boost::uuids::uuid uuid;
};

} // end namespace rpc
} // end namespace coral

#endif // RPC_COMMON_H

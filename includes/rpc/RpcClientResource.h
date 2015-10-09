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



#ifndef RPC_CLIENT_RESOURCE_H
#define RPC_CLIENT_RESOURCE_H

#include <string>
#include "RpcObject.h"
#include "RpcException.h"
#include "RpcClient.h"
#include "AsyncRpcSupervisor.h"
#include <boost/uuid/uuid.hpp>


namespace liber {
namespace rpc {

class RpcClientResource {
public:

   RpcClientResource(RpcClient &client, const std::string &classname);
   virtual ~RpcClientResource();
   
   enum { InvalidInstance = -1 };

   void setTimeout(int nTimeoutMs);
 
   RpcException getLastError();

protected:
   
   void setCallTimeout(ui32 nTimeoutMs);
   
   bool construct();
   
   bool destroy();
   
   bool call(const std::string&  methodName,
             const PbMessage&    request,
             PbMessage&          response,
             AsyncRpcSupervisor* pSupervisor = NULL);

   void marshallRequest(RpcObject&         requestObject,
                        const std::string& methodName,
                        const PbMessage*   pReqeustParameters = NULL);
   
private:
   
   bool invoke(const RpcObject& object,
               RpcObject&       result,
               ui32             nTimeoutMs);

protected:

   RpcClient &mrClient;
   
   std::string mClassname;
   
   boost::uuids::uuid mUuid;

   RpcException mLastError;

   int mnTimeoutMs;
};

}}

#endif // RPC_CLIENT_RESOURCE_H

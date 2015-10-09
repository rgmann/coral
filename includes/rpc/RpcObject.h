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



#ifndef RPC_OBJECT_H
#define RPC_OBJECT_H

#include <string>
#include <google/protobuf/message.h>
#include "Serializable.h"
#include "RpcException.h"

namespace liber {
namespace rpc {

typedef ::google::protobuf::Message PbMessage;


class RpcObject : public liber::netapp::Serializable {
public:

   RpcObject();
   RpcObject(const std::string &resourceName, const std::string &actionName);
   virtual ~RpcObject();

   //--------------------------------------------------------------------------
   //                            Public Fields
   //--------------------------------------------------------------------------
   RpcCallInfo&  callInfo();
   const RpcCallInfo&  callInfo() const;
   RpcException& exception();
   const RpcException& exception() const;


   //--------------------------------------------------------------------------
   //                            Public Methods
   //--------------------------------------------------------------------------
   void setParams(const PbMessage& message);
   void setParams(const std::string& message);
   void getParams(PbMessage& message) const;
   void getParams(std::string& message) const;
   
   virtual bool isValid() const;
      
   
   bool getResponse(RpcObject &response) const;
   bool getResponse(RpcObject &response, const PbMessage &value) const;
   bool getResponse(RpcObject &response, const std::string &value) const;

   std::string  mMessage;

protected:

  void pack(liber::netapp::SerialStream&);
  void pack(liber::netapp::SerialStream&) const;
  bool unpack(liber::netapp::SerialStream&);

private:

   // Protobuf message serialized to a byte string.
   //std::string  mMessage;
   RpcCallInfo  mCallInfo;
   RpcException mException;
};

}}

#endif // RPC_OBJECT_H

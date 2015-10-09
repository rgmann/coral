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



#ifndef INSTANCE_WRAPPER_H
#define INSTANCE_WRAPPER_H

#include "RpcCommon.h"
#include "RpcException.h"

#define DELEGATE(action_delegate)                        \
  action_delegate(                                       \
              const std::string& request,                \
              std::string& response,                     \
              liber::rpc::RpcException& e)

#define ACTION(action_name, request_type, response_type) \
  action_name(const request_type& request,               \
              response_type& response,                   \
              liber::rpc::RpcException& e)


namespace liber {
namespace rpc {

class InstanceWrapper {
public:

  typedef void (*Method)( const std::string&, std::string&, RpcException& );
   
  InstanceWrapper(){};
  virtual ~InstanceWrapper(){};
   
  // virtual bool initialize(const std::string &params) = 0;
  // virtual bool destroy(const std::string &params) = 0;

  const boost::uuids::uuid& uuid();

private:

  boost::uuids::uuid mUUID;
};

}}

#endif // INSTANCE_WRAPPER_H

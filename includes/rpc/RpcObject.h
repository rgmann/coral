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

namespace coral {
namespace rpc {

typedef ::google::protobuf::Message PbMessage;

///
/// The RpcObject encapsulates an RPC request or an RPC response. The RpcObject
/// also maintains information about the call itself, including a descriptor
/// and an exception. The RpcObject is responsible for serializing/deserializing
/// requests and responses.
///
class RpcObject : public coral::netapp::Serializable {
public:

   ///
   /// Construct an empty RPC object
   ///
   RpcObject();

   ///
   /// Construct an RPC object for the specified resource/service method.
   ///
   /// @param  resource_name Resource/service name
   /// @param  action_name   Name of action that this object is associated with
   ///
   RpcObject(const std::string &resourceName, const std::string &actionName);

   virtual ~RpcObject();

   ///
   /// Get information about the call that this object is associated with.
   ///
   /// @return RpcCallInfo&  Call descriptor
   ///
   RpcCallInfo&  callInfo();
   const RpcCallInfo&  callInfo() const;

   ///
   /// Get the exception status associated with this object's call.
   ///
   /// @return RpcException& Exception information
   ///
   RpcException& exception();
   const RpcException& exception() const;


   ///
   /// Set the request or response message. The protocol buffer is serialized
   /// to a std::string.
   ///
   /// @param  message  Protocol buffer request or response message
   /// @return void
   ///
   void setParams( const PbMessage& message);

   ///
   /// Set the request or response message. The protocol buffer is serialized
   /// to a std::string.
   ///
   /// @param  message  Protocol buffer request or response message
   /// @return void
   ///
   void setParams( const std::string& message);

   ///
   /// Get the request or response message.
   ///
   /// @return PbMessage&  Deserialized protocol buffer request/response message
   ///
   void getParams( PbMessage& message) const;

   ///
   /// Get the request or response message.
   ///
   void getParams( std::string& message) const;

   ///
   /// Query whether the object represents a valid request or response.
   ///
   /// @return bool True if the object represents a valid request or response;
   ///              false if the object is empty.
   ///
   virtual bool isValid() const;
      
   
   ////////////////////////////////////////////////////////////////////////////
   /// The following public methods are used solely by the RpcServer and
   /// RpcServerResource to build a response RpcObject for a request RpcObject.
   ////////////////////////////////////////////////////////////////////////////

   ///
   /// Create a response RpcObject for a request RpcObject.
   ///
   /// @param  response  Response RpcObject
   /// @param  message   Message to be serialized in response
   /// @return bool      False if the response cannot be built because this
   ///                   (the request) is not valid; true otherwise
   ///
   bool getResponse( RpcObject& response) const;
   bool getResponse( RpcObject& response, const PbMessage& message ) const;
   bool getResponse( RpcObject& response, const std::string& messag ) const;

protected:

   void pack(coral::netapp::SerialStream&) const;
   bool unpack(coral::netapp::SerialStream&);

private:

   RpcCallInfo  call_info_;
   RpcException exception_;

   std::string message_;
};

}}

#endif // RPC_OBJECT_H

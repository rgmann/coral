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



#ifndef RPC_EXCEPTION_H
#define RPC_EXCEPTION_H

#include <string>
#include <vector>
#include "Serializable.h"
#include "RpcCommon.h"

namespace coral {
namespace rpc {

class TraceFrame : public coral::netapp::Serializable {
public:

   std::string mClassName;
   std::string mMethodName;
   std::string mFileName;
   i32         mLineNumber;

   TraceFrame(const std::string& className = "",
              const std::string& methodName = "",
              const std::string& filename = "",
              int lineNumber = -1);

   std::string toString() const;


protected:

   /**
    * Serialize the Trace Frame to a binary string.
    */
   void pack(coral::netapp::SerialStream&);
   void pack(coral::netapp::SerialStream&) const;

   /**
    * Deserialize a Trace Frame from a binary string.
    * The current instance (this) is modified.
    *
    * @return bool True if successful; false otherwise.
    */
   bool unpack(coral::netapp::SerialStream&);
};


class RpcException : public coral::netapp::Serializable {
public:

   RpcException();

   RpcErrorId  id;
   std::string message;

   const RpcCallInfo& callInfo() const;

   enum Reporter
   {
      Unknown,
      Server,
      Client
   };
   Reporter reporter;

   std::string toString() const;

   bool valid() const;

   void pushFrame(const TraceFrame& frame);
   void popFrame();
   const std::vector<TraceFrame>& frameTrace() const;
   std::vector<std::string> textTrace() const;
   std::string trace() const;

   bool pushTrace(const RpcException& other);

   void reset();

protected:

  void pack(coral::netapp::SerialStream&);
  void pack(coral::netapp::SerialStream&) const;
  bool unpack(coral::netapp::SerialStream&);

private:

   RpcCallInfo mCallInfo;
   std::vector<TraceFrame> mTrace;

   friend class RpcObject;
};

}}

#endif // RPC_EXCEPTION_H

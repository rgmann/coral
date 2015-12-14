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

   std::string resource_name_;
   std::string method_name_;
   std::string filename_;
   i32         line_number_;

   TraceFrame( const std::string& resource_name = "",
               const std::string& method_name = "",
               const std::string& filename = "",
               int                line_number = -1);

   ///
   /// Format the trace frame as a string.
   ///
   /// @return std::string  Trace frame string
   ///
   std::string toString() const;


protected:

   ///
   /// Serialize the Trace Frame to a binary string.
   ///
   void pack( coral::netapp::SerialStream& ) const;

   ///
   /// Deserialize a Trace Frame from a binary string.
   /// The current instance (this) is modified.
   ///
   /// @return bool True if successful; false otherwise.
   ///
   bool unpack(coral::netapp::SerialStream&);

};


class RpcException : public coral::netapp::Serializable {
public:

   RpcException();

   RpcErrorId  id;
   std::string message;


   enum Reporter {
      kUnknown,
      kServer,
      kClient
   };

   Reporter reporter;

   ///
   /// Remote procedure call descriptor
   ///
   /// @return RpcCallInfo  Info for call that generated the exception
   ///
   const RpcCallInfo& callInfo() const;

   ///
   /// Format the exception as string.
   ///
   /// @return std::string  Exception string
   ///
   std::string toString() const;

   ///
   /// Check whether the the RpcException is valid (represents a de-serialized
   /// exception).
   ///
   /// @return bool  True if the exception is valid; false otherwise.
   ///
   bool valid() const;

   ///
   /// Add a new frame to the exception frame stack.
   ///
   /// @param  frame  Trace frame
   /// @return void
   ///
   void pushFrame(const TraceFrame& frame);

   ///
   /// Pop the trace frame at the top of the trace stack.
   ///
   /// @return void
   ///
   void popFrame();

   ///
   /// Get the full frame trace
   ///
   /// @return std::vector<TraceFrame>  TraceFrame stack
   ///
   const std::vector<TraceFrame>& frameTrace() const;

   ///
   /// Get the full frame trace as strings
   ///
   /// @return std::vector<std::string> TraceFrame stack 
   ///
   std::vector<std::string> textTrace() const;

   ///
   /// Get the trace frame as a string.
   ///
   /// @return std::string  Trace frame string
   ///
   std::string trace() const;

   ///
   /// Append the exception trace from the other RPC exception.
   ///
   /// @param  other  
   /// @return void
   ///
   void pushTrace( const RpcException& other );

   ///
   /// Clear the exception
   ///
   void reset();


protected:

  void pack( coral::netapp::SerialStream& ) const;
  bool unpack( coral::netapp::SerialStream& );

private:

   RpcCallInfo call_info_;

   std::vector<TraceFrame> trace_;

   friend class RpcObject;
};

} // end namespace rpc
} // end namespace coral

#endif // RPC_EXCEPTION_H

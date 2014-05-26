#ifndef RPC_EXCEPTION_H
#define RPC_EXCEPTION_H

#include <string>
#include <vector>
#include "RpcCommon.h"

namespace liber {
namespace rpc {

class TraceFrame {
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

   /**
    * Serialize the Trace Frame to a binary string.
    */
   std::string serialize() const;

   /**
    * Deserialize a Trace Frame from a binary string.
    * The current instance (this) is modified.
    *
    * @return bool True if successful; false otherwise.
    */
   bool deserialize(const std::string& data);
};


class RpcException {
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

   std::string serialize() const;
   bool deserialize(const std::string& data);

   bool valid() const;

   void pushFrame(const TraceFrame& frame);
   void popFrame();
   const std::vector<TraceFrame>& frameTrace() const;
   std::vector<std::string> textTrace() const;
   std::string trace() const;

   bool pushTrace(const RpcException& other);

   void reset();

private:

   RpcCallInfo mCallInfo;
   std::vector<TraceFrame> mTrace;

   friend class RpcObject;
};

}}

#endif // RPC_EXCEPTION_H

#ifndef RPC_MARSHALLED_CALL_H
#define RPC_MARSHALLED_CALL_H

#include <time.h>
#include "BinarySem.h"
#include "RpcObject.h"
#include "RpcPacket.h"

namespace liber {
namespace rpc {

class RpcMarshalledCall {
public:

   RpcMarshalledCall();
   
   RpcMarshalledCall(const RpcObject &object);
   
   RpcPacket* getRpcPacket() const;

   void getResult(RpcObject &result);

   const RpcObject& input() const;

   void notify(const RpcObject &object);
   void cancel();
   bool cancelled() const;

   bool wait( ui32 timeout_ms, bool cancel_on_timeout = true );
   
   // Indicates that the caller is no longer waiting.  This call can be
   // garbage collected.
   void dispose();
   bool isDisposed() const;
   
   RpcObject& getResultObject();

private:
   
   BinarySem mSem;

   RpcObject param_object_;
   RpcObject result_object_;
 
   bool is_disposed_;
   bool cancelled_;
};

}}

#endif // RPC_MARSHALLED_CALL_H

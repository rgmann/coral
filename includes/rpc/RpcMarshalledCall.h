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

   i64 getRpcId() const;

   void notify(const RpcObject &object);
   void cancel();
   bool cancelled() const;

   bool wait(ui32 nTimeoutMs, bool bCancelOnTimeout = true);
   
   // Indicates that the caller is no longer waiting.  This call can be
   // garbage collected.
   void dispose();
   bool isDisposed() const;
   
   RpcObject& getResultObject();

private:
   
   static i64 ourCurrentRpcId;

   BinarySem mSem;

   RpcObject mParamObj;
   RpcObject mResultObj;
 
   bool mbIsDisposed;
   bool mbCancelled;
};

}}

#endif // RPC_MARSHALLED_CALL_H

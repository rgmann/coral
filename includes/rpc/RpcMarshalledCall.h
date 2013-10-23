#ifndef RPC_MARSHALLED_CALL_H
#define RPC_MARSHALLED_CALL_H

#include <time.h>
#include "BinarySem.h"
#include "RpcObject.h"
#include "RpcPacket.h"

class RpcMarshalledCall
{
public:

   RpcMarshalledCall();
   
   RpcMarshalledCall(const RpcObject &object);
   
   void getRpcPacket(RpcPacket** pPacket) const;

   void getResult(RpcObject &result);

   i64 getRpcId() const;

//   bool compareRpcId(const RpcId &id);

   void notify(const RpcObject &object);

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
};

#endif // RPC_MARSHALLED_CALL_H

#include <iostream>
#include "RpcMarshalledCall.h"

using namespace liber::rpc;

i64 RpcMarshalledCall::ourCurrentRpcId = 0;

//-----------------------------------------------------------------------------
RpcMarshalledCall::RpcMarshalledCall(const RpcObject &object)
: mbIsDisposed(false)
, mbCancelled(false)
{
   mParamObj = object;
   mParamObj.callInfo().rpcId = ++ourCurrentRpcId;
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::getRpcPacket(RpcPacket** pPacket) const
{
   *pPacket = new RpcPacket(mParamObj);
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::getResult(RpcObject &result)
{
   result = mResultObj;
}

//-----------------------------------------------------------------------------
i64 RpcMarshalledCall::getRpcId() const
{
   return mParamObj.callInfo().rpcId;
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::dispose()
{
   mbIsDisposed = true;
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::isDisposed() const
{
   return mbIsDisposed;
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::notify(const RpcObject &object)
{
   mResultObj = object;
   mSem.give();
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::cancel()
{
  mbCancelled = true;
  mSem.give();
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::cancelled() const
{
  return mbCancelled;
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::wait(ui32 nTimeoutMs, bool bCancelOnTimeout)
{
   bool lbSuccess = mSem.take(nTimeoutMs);

   if (!lbSuccess && bCancelOnTimeout)
   {
     mbIsDisposed = true;
   }

   return lbSuccess;
}



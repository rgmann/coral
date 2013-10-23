#include <iostream>
#include "RpcMarshalledCall.h"
#include "JsonTransportObject.h"

i64 RpcMarshalledCall::ourCurrentRpcId = 0;

//-----------------------------------------------------------------------------
RpcMarshalledCall::RpcMarshalledCall(const RpcObject &object)
: mbIsDisposed(false)
{
   mParamObj = object;
   mParamObj.setRpcId(++ourCurrentRpcId);
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::getRpcPacket(RpcPacket** pPacket) const
{
   JsonTransportObject lTransportObject(mParamObj);
   /*std::cout << "RpcMarshalledCall::getRpcPacket:" << std::endl
             << lTransportObject.getString()
             << std::endl;*/
   *pPacket = new RpcPacket(lTransportObject);
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::getResult(RpcObject &result)
{
   result = mResultObj;
}

//-----------------------------------------------------------------------------
i64 RpcMarshalledCall::getRpcId() const
{
   return mParamObj.getRpcId();
}

//-----------------------------------------------------------------------------
/*bool RpcMarshalledCall::compareRpcId(const RpcId &id)
{
}*/

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
bool RpcMarshalledCall::wait(ui32 nTimeoutMs, bool bCancelOnTimeout)
{
   bool lbSuccess = mSem.take(nTimeoutMs);

   if (!lbSuccess && bCancelOnTimeout) mbIsDisposed = true;

   return lbSuccess;
}



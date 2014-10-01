#include "Log.h"
#include "RpcClient.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
RpcClient::RpcClient()
{
}

//-----------------------------------------------------------------------------
RpcClient::~RpcClient()
{
   std::map<i64, RpcMarshalledCall*>::iterator lRpcIt;

   mRpcMutex.lock();

   lRpcIt = mRpcMap.begin();
   while (lRpcIt != mRpcMap.end())
   {
      if (lRpcIt->second)
      {
         delete lRpcIt->second;
      }

      mRpcMap.erase(lRpcIt++);
   }

   mRpcMutex.unlock();
}

//-----------------------------------------------------------------------------
RpcMarshalledCall* RpcClient::invokeRpc(const RpcObject &object)
{
  RpcMarshalledCall* lpCall = new (std::nothrow) RpcMarshalledCall(object);

  if (lpCall)
  {
    if (mRpcMap.count(lpCall->getRpcId()) == 0)
    {
      mRpcMutex.lock();
      mRpcMap.insert(std::make_pair(lpCall->getRpcId(), lpCall));
      mRpcMutex.unlock();
      
      RpcPacket* lpPacket = lpCall->getRpcPacket();

      // The RPC client and server always have the same subscriber ID,
      // so there is no need to send the packet to a particular subscriber ID.
      sendPacket(lpPacket);
    }
    else
    {
      log::error("RpcClient::invokeRpc - "
                 "RPC call with same UUID already exists\n");
    }
  }
  else
  {
    log::error("RpcClient::invokeRpc - Failed to create RpcMarshalledCall\n");
  }

  return lpCall;
}

//-----------------------------------------------------------------------------
bool RpcClient::put(const char* pData, ui32 nLength)
{
  bool lbSuccess = false;
  RpcPacket* lpPacket = new RpcPacket();
  RpcObject lRxObject;

  if (lpPacket->unpack(pData, nLength))
  {
    if (lpPacket->getObject(lRxObject))
    {
      RpcMarshalledCall* lpCall = NULL;
      
      if (mRpcMap.count(lRxObject.callInfo().rpcId) > 0)
      {
        lpCall = mRpcMap.find(lRxObject.callInfo().rpcId)->second;
      }

      if (lpCall)
      {
        lpCall->notify(lRxObject);
        lbSuccess = true;
      }
      else
      {
        log::error("Failed to find RPC with ID = %d\n", 
                   lRxObject.callInfo().rpcId);
      }
    }
    else
    {
      log::error("RpcClient::put: Failed to get RpcObject\n");
    }
  }
  else
  {
    log::error("RpcClient::put: Failed to unpack RpcPacket\n");
  }

  processCancellations();
  delete lpPacket;

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void RpcClient::processCancellations()
{
  std::map<i64, RpcMarshalledCall*>::iterator lRpcIt;

  mRpcMutex.lock();
  lRpcIt = mRpcMap.begin();
  while (lRpcIt != mRpcMap.end())
  {
    if (lRpcIt->second->isDisposed())
    {
      if (lRpcIt->second)
      {
        delete lRpcIt->second;
      }

      mRpcMap.erase(lRpcIt++);
    }
    else
    {
      ++lRpcIt;
    }
  }
  mRpcMutex.unlock();
}


#include <iostream>
#include "RpcClient.h"

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
  RpcMarshalledCall* lpCall = new RpcMarshalledCall(object);

  if (mRpcMap.count(lpCall->getRpcId()) == 0)
  {
    mRpcMutex.lock();
    mRpcMap[lpCall->getRpcId()] = lpCall;
    mRpcMutex.unlock();
      
    RpcPacket* lpPacket = NULL;
    lpCall->getRpcPacket(&lpPacket);

    sendPacket(lpPacket);
  }

  return lpCall;
}

//-----------------------------------------------------------------------------
bool RpcClient::put(const char* pData, ui32 nLength)
{
  bool lbSuccess = false;
  RpcPacket* lpPacket = new RpcPacket();
  RpcObject lRxObject;

  lpPacket->unpack(pData, nLength);

  if (lpPacket->getObject(lRxObject))
  {
    RpcMarshalledCall* lpCall = NULL;
      
    if (mRpcMap.count(lRxObject.callInfo().rpcId) > 0)
    {
      lpCall = mRpcMap[lRxObject.callInfo().rpcId];
    }

    if (lpCall)
    {
      lpCall->notify(lRxObject);
      lbSuccess = true;
    }
    else
    {
      std::cout << "Failed to find RPC with ID = " 
                << lRxObject.callInfo().rpcId 
                << std::endl;
    }
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


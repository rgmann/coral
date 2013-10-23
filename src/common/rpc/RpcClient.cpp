#include "RpcClient.h"
#include "JsonTransportObject.h"

//-----------------------------------------------------------------------------
RpcClient::RpcClient()
{
   mOutQueue.initialize();
}

//-----------------------------------------------------------------------------
RpcClient::~RpcClient()
{
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
            
      bool lbSuccess = mOutQueue.push(lpPacket);
   }

   return lpCall;
}

//-----------------------------------------------------------------------------
bool RpcClient::processPacket(const RpcPacket* pPacket)
{
   bool lbSuccess = false;
   JsonTransportObject lTransportObject;

   if (pPacket->getObject(lTransportObject))
   {
      RpcObject lRpcObject;
      RpcMarshalledCall* lpCall = NULL;
      
      lTransportObject.to(lRpcObject);
      
      if (mRpcMap.count(lRpcObject.getRpcId()) > 0)
      {
         lpCall = mRpcMap[lRpcObject.getRpcId()];
      }

      if (lpCall)
      {
         lpCall->notify(lRpcObject);
         lbSuccess = true;
      }
      else
      {
         std::cout << "Failed to find RPC" << std::endl;
      }
   }

   processCancellations();

   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RpcClient::popPacket(RpcPacket** pPacket)
{
   return mOutQueue.pop(*pPacket, 100);
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
         if (lRpcIt->second) delete lRpcIt->second;
         mRpcMap.erase(lRpcIt++);
      }
      else
      {
         ++lRpcIt;
      }
   }
   mRpcMutex.unlock();
}


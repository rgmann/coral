#include <iostream>
#include "RpcServer.h"
#include "Log.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::netapp;

//------------------------------------------------------------------------------
RpcServer::RpcServer()
{
}

//------------------------------------------------------------------------------
bool RpcServer::registerResource(RpcServerResource* pResource)
{
   if (pResource == NULL) return false;
   if (mResourceMap.count(pResource->getName()) != 0) return false;

   mResourceMap[pResource->getName()] = pResource;
   pResource->registerActions();

   return true;
}

//------------------------------------------------------------------------------
bool RpcServer::processPacket(const RpcPacket* pPacket)
{
   bool lbSuccess = false;
   RpcObject lInput;

   if (pPacket && pPacket->getObject(lInput))
   {
      RpcObject lOutput;
      RpcServerResource* lpResource = getResource(lInput);

      lInput.exception().pushFrame(TraceFrame("RpcServer", "processPacket",
                                              __FILE__, __LINE__));

      if (lpResource)
      {
         lbSuccess = lpResource->unmarshall(lInput, lOutput);
      }
      else
      {
         lInput.exception().reporter = RpcException::Server;
         lInput.exception().id = InvalidUIID;

         lInput.getResponse(lOutput);
      }

      lInput.exception().popFrame();
                   
      sendObject(lOutput);
   }

   return lbSuccess;
}

//------------------------------------------------------------------------------
bool RpcServer::put(const char* pData, ui32 nLength)
{
  bool lbSuccess = false;
  RpcPacket* lpPacket = new RpcPacket();

  if (pData)
  {
    lbSuccess = lpPacket->unpack(pData, nLength);
    if (lbSuccess)
    {
      lbSuccess = processPacket(lpPacket);
    }
    else
    {
      log::error("RpcServer::put: Fail to unpack packet of size %u\n", nLength);
    }
  }

  return lbSuccess;
}

//------------------------------------------------------------------------------
RpcServerResource* RpcServer::getResource(const RpcObject &object)
{
   RpcServerResource* lpResource = NULL;

   if (mResourceMap.count(object.callInfo().resource) == 1)
   {
      lpResource = mResourceMap[object.callInfo().resource];
   }

   return lpResource;
}

//------------------------------------------------------------------------------
void RpcServer::sendObject(const RpcObject &object)
{
  sendPacket(new RpcPacket(object));
}



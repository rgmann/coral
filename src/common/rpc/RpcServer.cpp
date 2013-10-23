#include <iostream>
#include "RpcServer.h"
#include "JsonTransportObject.h"

//------------------------------------------------------------------------------
RpcServer::RpcServer()
{
   //mCommandQueue.initialize();
   mResponseQueue.initialize();
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
bool RpcServer::IsRpcCommand(const GenericPacket *pPacket)
{
   return true;
}

/*bool pushCommand(const RpcCommand* pCmd)
{
   return mCmdQueue.push(pCmd, 100);
}*/

//------------------------------------------------------------------------------
bool RpcServer::processPacket(const RpcPacket* pPacket)
{
   bool lbSuccess = false;
   JsonTransportObject lTransportObject;

   if (pPacket && pPacket->getObject(lTransportObject))
   {
      RpcObject lInput;
      RpcObject lOutput;
            
      lTransportObject.to(lInput);
      RpcServerResource* lpResource = getResource(lInput);

      if (lpResource)
      {
         lbSuccess = lpResource->unmarshall(lInput, lOutput);
      }
      else
      {
         lInput.getResponse(lOutput, InvalidInstanceId);
      }
                   
      sendObject(lOutput);
   }

   return lbSuccess;
}

//------------------------------------------------------------------------------
RpcServerResource* RpcServer::getResource(const RpcObject &object)
{
   RpcServerResource* lpResource = NULL;

   if (mResourceMap.count(object.getClass()) == 1)
   {
      lpResource = mResourceMap[object.getClass()];
   }

   return lpResource;
}

//------------------------------------------------------------------------------
void RpcServer::sendObject(const RpcObject &object)
{
   JsonTransportObject lTransportObj(object);
   mResponseQueue.push(new RpcPacket(lTransportObj));
   //std::cout << "RpcServer::sendObject:" << std::endl << lTransportObj.getString() << std::endl;
}

//------------------------------------------------------------------------------
bool RpcServer::popPacket(RpcPacket** pPacket)
{
   return mResponseQueue.pop(*pPacket, 100);
}

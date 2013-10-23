#ifndef RPC_SERVER_H
#define RPC_SERVER_H

#include <map>
#include <string>
#include "RpcPacket.h"
#include "RpcServerResource.h"

class RpcServer
{
public:

   RpcServer();

   bool registerResource(RpcServerResource* pResource);

   static bool IsRpcCommand(const GenericPacket *pPacket);

   bool processPacket(const RpcPacket* pPacket);
   
   bool popPacket(RpcPacket** pPacket);

   //bool pushCommand(const RpcCommand* pCmd);

   //void setAckQueue(Queue<GenericPacket*>* pQueue);

private:

   RpcServerResource* getResource(const RpcObject &object);

   //bool invoke(RpcObject &object);
   void sendObject(const RpcObject &object);

private:

   std::map<std::string, RpcServerResource*> mResourceMap;

   //Queue<RpcCommand*>      mCmdQueue;
   Queue<RpcPacket*>   mResponseQueue;
};

#endif // RPC_SERVER_H

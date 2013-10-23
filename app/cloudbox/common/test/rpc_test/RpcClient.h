#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <map>
#include "Queue.h"
#include "Mutex.h"
#include "RpcPacket.h"
#include "RpcMarshalledCall.h"

class RpcClient
{
public:

   RpcClient();
   
   ~RpcClient();

   RpcMarshalledCall* invokeRpc(const RpcObject &object);
   void dispose(RpcMarshalledCall& call);

   static bool IsRpcPacket(const GenericPacket* pPacket);

   bool processPacket(const RpcPacket* pPacket);

   bool popPacket(RpcPacket** pPacket);

private:

   RpcMarshalledCall* getMarshalledRpc(const RpcObject &object);

   void processCancellations();

private:

   Queue<RpcPacket*> mOutQueue;

   Mutex mRpcMutex;
   
   // Map each call to a unique identifier so that we can look up the
   // call when a response is received.
   std::map<i64, RpcMarshalledCall*> mRpcMap;
};

#endif // RPC_CLIENT_H

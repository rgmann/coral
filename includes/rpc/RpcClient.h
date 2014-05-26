#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <map>
#include "Queue.h"
#include "Mutex.h"
#include "RpcPacket.h"
#include "RpcMarshalledCall.h"
#include "PacketSubscriber.h"

namespace liber {
namespace rpc {

class RpcClient : public liber::netapp::PacketSubscriber {
public:

   RpcClient();
   
   ~RpcClient();

   RpcMarshalledCall* invokeRpc(const RpcObject &object);

   bool put(const char* pData, ui32 nLength);

private:

   RpcMarshalledCall* getMarshalledRpc(const RpcObject &object);

   void processCancellations();

private:

   Mutex mRpcMutex;
   
   // Map each call to a unique identifier so that we can look up the
   // call when a response is received.
   std::map<i64, RpcMarshalledCall*> mRpcMap;
};

}}

#endif // RPC_CLIENT_H

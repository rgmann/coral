#ifndef RPC_SERVER_H
#define RPC_SERVER_H

#include <map>
#include <string>
#include "RpcPacket.h"
#include "RpcServerResource.h"
#include "PacketSubscriber.h"

namespace liber {
namespace rpc {

class RpcServer : public liber::netapp::PacketSubscriber {
public:

  RpcServer();

  bool registerResource(RpcServerResource* pResource);

  bool processPacket(const RpcPacket* pPacket);
  bool put(const char* pData, ui32 nLength);

private:

  RpcServerResource* getResource(const RpcObject &object);

  void sendObject(const RpcObject &object);

private:

  std::map<std::string, RpcServerResource*> mResourceMap;
};

}}

#endif // RPC_SERVER_H

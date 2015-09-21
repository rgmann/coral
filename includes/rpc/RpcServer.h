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

  RpcServer( liber::netapp::DestinationID client_destination_id );

  bool registerResource( RpcServerResource* resource_ptr );

  bool processPacket(const RpcPacket* packet_ptr );
  bool put( liber::netapp::DestinationID, const void* data_ptr, ui32 length );

private:

  RpcServerResource* getResource(const RpcObject &object);

  void sendObject(const RpcObject &object);

private:

  typedef  std::map<std::string, RpcServerResource*>  ResourceMap;

  ResourceMap resources_;

  liber::netapp::DestinationID client_destination_id_;
};

}}

#endif // RPC_SERVER_H

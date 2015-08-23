#ifndef PACKET_RECEIVER_H
#define PACKET_RECEIVER_H

namespace liber {
namespace netapp {

typedef unsigned int DestinationID;

class PacketContainer {
public:

  PacketContainer();

  PacketContainer( DestinationID destination_id, class GenericPacket* packet_ptr );

  DestinationID destination_id_;
  class GenericPacket* packet_ptr_;
};


class PacketReceiver {
public:

  PacketReceiver() {};
  virtual ~PacketReceiver() {};

  virtual bool send( PacketContainer* container_ptr ) = 0;
};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_RECEIVER_H

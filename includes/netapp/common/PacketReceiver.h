#ifndef PACKET_RECEIVER_H
#define PACKET_RECEIVER_H

namespace liber {
namespace netapp {

class PacketContainer {
public:

  PacketContainer();

  PacketContainer(int destination, class GenericPacket* pPacket);

  int mDestinationID;
  class GenericPacket* mpPacket;
};


class PacketReceiver {
public:

  PacketReceiver() {};
  virtual ~PacketReceiver() {};

  virtual bool push(PacketContainer* pContainer) = 0;
};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_RECEIVER_H

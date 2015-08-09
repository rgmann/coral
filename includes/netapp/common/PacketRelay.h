#ifndef PACKET_RELAY_H
#define PACKET_RELAY_H


#include "PacketReceiverHook.h"
#include "PacketRouter.h"
#include "PacketSubscriber.h"


namespace liber {
namespace netapp {

// The RelayReceiverHook class simply receives PacketContainers from
// subscribers, translates/repackages them, and then "relays" them to
// the supplied PacketReceiver.
class RelayReceiverHook : public PacketReceiverHook {
public:

  RelayReceiverHook();
  virtual ~RelayReceiverHook();

  void setReceiver(PacketReceiver* pReceiver);

protected:

  bool call(PacketContainer* pContainer);

  virtual PacketContainer* translate(PacketContainer* pContainer);

private:

  PacketReceiver* receiver_ptr_;
};


// The PacketRelay class allows a multiple-subscriber application to share a
// single subscriber ID. All packets leaving the subscribers will become the
// payload of packet with the PacketRelay's subscriber ID. Incomming packets
// are multiplexed to the subscribers register with the PacketRelay.
class PacketRelay : public PacketRouter, public PacketSubscriber {
public:

  /**
   * The PacketRelay depends on the ReceiverHook to offload reformatting and
   * transmission of outgoing packets to the subscriber's thread context.
   * Any number of PacketRelay subscriber's can transmit packets
   * simultaneously.
   */
  explicit PacketRelay(RelayReceiverHook& rInReceiver);

  virtual ~PacketRelay();

  void setReceiver(PacketReceiver* pReceiver);

  bool put(const char* pData, ui32 nSizeBytes);

protected:

  /**
   * Pure virtual method to extract an encapsulated packet and return a
   * routable PacketContainer. This method assumes that the received
   * represents a packet parsable by this subscriber. The packet header
   * should indicate the PacketRelay subscriber that should receiver the packet
   * payload. The method is virtual to allow flexibility in how the packet
   * header is defined.
   */
  virtual PacketContainer* toContainer(const char* pData, ui32 nSizeBytes) = 0;

private:

  RelayReceiverHook& mrInReceiver;
};


} // End namespace netapp
} // End namespace liber

#endif // PACKET_RELAY_H

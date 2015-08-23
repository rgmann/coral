#ifndef PACKET_RECEIVER_HOOK_H
#define PACKET_RECEIVER_HOOK_H

#include "PacketReceiver.h"

namespace liber {
namespace netapp {

class PacketReceiverHook : public PacketReceiver {
public:

  PacketReceiverHook();
  virtual ~PacketReceiverHook();

  bool send( PacketContainer* container_ptr );

protected:

  virtual bool call( PacketContainer* container_ptr ) = 0;

};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_RECEIVER_HOOK_H

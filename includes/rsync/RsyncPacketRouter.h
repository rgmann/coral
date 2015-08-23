#ifndef RSYNC_PACKET_ROUTER_H
#define RSYNC_PACKET_ROUTER_H

#include "PacketReceiverHook.h"
#include "PacketRelay.h"

namespace liber {
namespace rsync {

class RsyncPacketReceiverHook : public liber::netapp::RelayReceiverHook {
public:

  RsyncPacketReceiverHook();

  void setSubscriberID(int subscriberID);

protected:

  liber::netapp::PacketContainer* translate(
    liber::netapp::DestinationID destination_id,
    liber::netapp::PacketContainer* in_container_ptr );

private:

  int mnSubscriberID;
};

class RsyncPacketRouter : public liber::netapp::PacketRelay {
public:

  RsyncPacketRouter();
  ~RsyncPacketRouter();

  void setID(int id);

protected:

  // liber::netapp::PacketContainer* toContainer(const char* pData, ui32 nSizeBytes);
  RelayInfo* extract( const RelayInfo& input );

private:

  RsyncPacketReceiverHook mReceiver;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_PACKET_ROUTER_H

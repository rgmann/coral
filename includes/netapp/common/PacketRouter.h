#ifndef PACKET_ROUTER_H
#define PACKET_ROUTER_H

#include <map>
#include "Queue.h"
#include "PacketSubscriber.h"

namespace liber {
namespace netapp {

class PacketRouter {
public:

  PacketRouter(Queue<liber::netapp::NetAppPacket*>* pOutQueue);
  virtual ~PacketRouter();

  bool addSubscriber(int subscriberId, liber::netapp::PacketSubscriber* pSubscriber);
  liber::netapp::PacketSubscriber* removeSubscriber(int subscriberId);

  ui32 count();
  bool empty();

protected:

  bool hasSubscriber(int subscriberId);
  liber::netapp::PacketSubscriber* getSubscriber(int subscriberId);

  //
  Queue<liber::netapp::NetAppPacket*>* mpOutQueue;

private:

  //
  Mutex mTableLock;

  //
  std::map<int, liber::netapp::PacketSubscriber*> mSubscriberTable;
};

}}

#endif // PACKET_ROUTER_H


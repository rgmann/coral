#ifndef PACKET_ROUTER_H
#define PACKET_ROUTER_H

#include <map>
#include "Thread.h"
#include "Socket.h"
#include "Mutex.h"
#include "Queue.h"
#include "NetAppPacket.h"
#include "PacketSubscriber.h"

namespace liber {
namespace netapp {

class ServerPacketRouter {
public:

  ServerPacketRouter(Queue<NetAppPacket*>* pInQueue,
                     Queue<NetAppPacket*>* pOutQueue);
  virtual ~ServerPacketRouter();

  bool addSubscriber(int subscriberId, liber::netapp::PacketSubscriber* pSubscriber);
  liber::netapp::PacketSubscriber* removeSubscriber(int subscriberId);

  bool route(netapp::NetAppPacket* pPacket);
  void routeFromQueue(i32 nSliceTimeMs);

private:

  int mnDequeueTimeoutMs;

  Queue<netapp::NetAppPacket*>* mpInQueue;
  Queue<netapp::NetAppPacket*>* mpOutQueue;

  Mutex mTableLock;
  std::map<int, liber::netapp::PacketSubscriber*> mSubscriberTable;
};

}}

#endif


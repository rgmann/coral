#ifndef CLIENT_PACKET_ROUTER_H
#define CLIENT_PACKET_ROUTER_H

#include "Thread.h"
#include "Socket.h"
#include "Mutex.h"
#include "Queue.h"
#include "PacketRouter.h"

namespace liber {
namespace netapp {

class ClientPacketRouter : public liber::netapp::PacketRouter {
public:

  static const ui32 DefaultQueueCapacity = 10;

  ClientPacketRouter(liber::net::Socket& rSocket);
  virtual ~ClientPacketRouter();

//  bool addSubscriber(int subscriberId, liber::netapp::PacketSubscriber* pSubscriber);
//  liber::netapp::PacketSubscriber* removeSubscriber(int subscriberId);

  bool start(ui32 txCapacity = DefaultQueueCapacity);
  void stop();

  // TODO: Add method to set logger.

  liber::net::Socket& socket();

  void setReadTimeout(int nTimeoutMs);

protected:

  void txThreadRun(ThreadArg*);
  void rxThreadRun(ThreadArg*);

private:

  static void TxThreadEntry(ThreadArg*);
  static void RxThreadEntry(ThreadArg*);

private:

  liber::net::Socket& mrSocket;
  Thread* mpTxThread;
  Thread* mpRxThread;

  int mnReadTimeoutMs;

  Queue<netapp::NetAppPacket*> mTxQueue;

//  Mutex mTableLock;
//  std::map<int, liber::netapp::PacketSubscriber*> mSubscriberTable;
};

}}

#endif // CLIENT_PACKET_ROUTER_H


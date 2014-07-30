#ifndef CLIENT_PACKET_ROUTER_H
#define CLIENT_PACKET_ROUTER_H

#include "Thread.h"
#include "Mutex.h"
#include "Queue.h"
#include "PacketQueue.h"
#include "PacketRouter.h"
#include "ClientConnectionStatus.h"

namespace liber {
namespace netapp {

class ClientPacketRouter : public liber::netapp::PacketRouter {
public:

  static const ui32 DefaultQueueCapacity = 10;

  ClientPacketRouter();
  virtual ~ClientPacketRouter();

//  bool addSubscriber(int subscriberId, liber::netapp::PacketSubscriber* pSubscriber);
//  liber::netapp::PacketSubscriber* removeSubscriber(int subscriberId);

  virtual bool start(ui32 txCapacity = DefaultQueueCapacity);
  void stop();

  // TODO: Add method to set logger.

  void setReadTimeout(int nTimeoutMs);

  void enableKeepalive(bool bEnable);

protected:

  virtual bool readPacket(NetAppPacket& rPacket, int nTimeoutMs) = 0;
  virtual bool writePacket(const PacketContainer& rPacket) = 0;

  void txThreadRun(ThreadArg*);
  void rxThreadRun(ThreadArg*);
  void routerThreadRun(ThreadArg*);

private:

  static void TxThreadEntry(ThreadArg*);
  static void RxThreadEntry(ThreadArg*);
  static void RouterThreadEntry(ThreadArg*);

private:

  bool mbKeepaliveEnabled;
  ClientConnectionStatus mConnectionStatus;

  Thread* mpTxThread;
  Thread* mpRxThread;
  Thread* mpRouterThread;

  int mnReadTimeoutMs;

  PacketQueue mTxQueue;
  Queue<NetAppPacket*> mRxQueue;

//  Mutex mTableLock;
//  std::map<int, liber::netapp::PacketSubscriber*> mSubscriberTable;
};

}}

#endif // CLIENT_PACKET_ROUTER_H


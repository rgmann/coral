#ifndef PACKET_ROUTER_H
#define PACKET_ROUTER_H

#include <map>
#include "Queue.h"
//#include "PacketSubscriber.h"

namespace liber {
namespace netapp {

class PacketContainer;
class PacketSubscriber;
class PacketReceiver;

class PacketRouter {
public:

  PacketRouter(PacketReceiver* pReceiver);
  virtual ~PacketRouter();

  bool addSubscriber(int subscriberId, PacketSubscriber* pSubscriber);
  PacketSubscriber* removeSubscriber(int subscriberId);

  ui32 count();
  bool empty();

protected:

  bool hasSubscriber(int subscriberId);
  PacketSubscriber* getSubscriber(int subscriberId);

  //
  PacketReceiver* mpReceiver;

private:

  //
  Mutex mTableLock;

  //
  std::map<int, PacketSubscriber*> mSubscriberTable;
};

}}

#endif // PACKET_ROUTER_H


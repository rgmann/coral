#ifndef PACKET_ROUTER_H
#define PACKET_ROUTER_H

#include <map>
#include <boost/thread/mutex.hpp>

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
  PacketReceiver* receiver_ptr_;

private:

  //
  boost::mutex table_lock_;

  //
  std::map<int, PacketSubscriber*> subscriber_table_;
};

}}

#endif // PACKET_ROUTER_H


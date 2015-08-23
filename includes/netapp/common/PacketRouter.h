#ifndef PACKET_ROUTER_H
#define PACKET_ROUTER_H

#include <map>
#include <boost/thread/mutex.hpp>
#include "PacketReceiver.h"
#include "PacketSubscriber.h"

namespace liber {
namespace netapp {

class PacketContainer;

//
// A Channel routes all associated packets to all subscribed endpoints
// (subscribers).
//
//
class Destination {
public:

   Destination( DestinationID destination_id ) : destination_id_( destination_id ){};

   DestinationID id() const { return destination_id_; }

   SubscriberList& subscribers(){ return subscribers_; }

   boost::mutex& lock(){ return lock_; }

private:

   DestinationID destination_id_;

   SubscriberList subscribers_;

   boost::mutex lock_;
};
typedef std::map<DestinationID,Destination*> DestinationTable;
typedef DestinationTable::iterator  DestinationIterator;

class PacketRouter {
public:

   PacketRouter(PacketReceiver* pReceiver);
   virtual ~PacketRouter();

   bool subscribe(
      DestinationID destination_id,
      PacketSubscriber* subscriber_ptr,
      SubscriberMode mode = kSubscriberModeReadWrite );
   bool unsubscribe( DestinationID destination_id, PacketSubscriber* subscriber_ptr );

   i32 count( DestinationID destination_id );
   bool empty();

   bool publish( DestinationID destination_id, const void* data_ptr, ui32 length );


protected:

   virtual bool allowPublish(
      DestinationID destination_id,
      PacketSubscriber* subscriber_ptr,
      const void* data_ptr, ui32 length ) { return true; }


  //
  PacketReceiver* receiver_ptr_;


private:

  //
  boost::mutex table_lock_;

  //
  DestinationTable destination_table_;
};

}}

#endif // PACKET_ROUTER_H


#ifndef PACKET_SUBSCRIBER_H
#define PACKET_SUBSCRIBER_H

#include <list>
#include <boost/thread/mutex.hpp>
#include "PacketReceiver.h"

namespace liber {
namespace netapp {

class GenericPacket;

enum SubscriberMode {
   kSubscriberModeReadOnly,
   kSubscriberModeReadWrite
};

struct RegisteredRouter {
   DestinationID     destination_id;
   SubscriberMode    mode;
   PacketReceiver*   receiver_ptr;

   RegisteredRouter() : receiver_ptr(NULL)
   {
   }
};

typedef  std::list<RegisteredRouter> RegisteredRouterList;
typedef  std::map<DestinationID,RegisteredRouter> RegisteredRouterMap;

class PacketSubscriber {
public:

   enum { kInvalidSubscriber = -1 };

   PacketSubscriber();
   virtual ~PacketSubscriber();

   virtual bool registerRouter( DestinationID destination_id, PacketReceiver* receiver_ptr, SubscriberMode mode );
   bool unregisterRouter( DestinationID destination_id );

   bool isSubscribed() const;

   /**
    * Synchronously process a packet.  If PacketApp does not perform any
    * synchronous packet processing, simply override with an implementation
    * that does nothing.
    */
   virtual bool put( DestinationID destination_id, const void* pData, ui32 length ) = 0;

   //
   // Send a packet to the specified destination channel
   //
   //
   //
   bool sendTo( DestinationID destination_id, GenericPacket* pPacket);


protected:

   virtual PacketReceiver* setReceiver( RegisteredRouter& router_info );


private:

   RegisteredRouterMap routers_;

   mutable boost::mutex subscribe_lock_;
};

typedef std::list<PacketSubscriber*>  SubscriberList;
typedef SubscriberList::iterator      SubscriberIterator;

} // End namespace netapp
} // End namespace liber

#endif // PACKET_SUBSCRIBER_H


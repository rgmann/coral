#ifndef PACKET_SUBSCRIBER_H
#define PACKET_SUBSCRIBER_H

#include <boost/thread/mutex.hpp>

namespace liber {
namespace netapp {

class GenericPacket;
class PacketReceiver;

class PacketSubscriber {
public:

   enum { kInvalidSubscriber = -1 };

   PacketSubscriber();
   virtual ~PacketSubscriber();

   virtual void setReceiver(PacketReceiver* receiver_ptr);

   virtual void setID(int id);

   void subscribe( int subscriber_id, PacketReceiver* receiver_ptr );
   void unsubscribe();

   bool isSubscribed() const;

   /**
    * Synchronously process a packet.  If PacketApp does not perform any
    * synchronous packet processing, simply override with an implementation
    * that does nothing.
    */
   virtual bool put(const char* pData, ui32 nLength) = 0;

   virtual bool sendPacket(GenericPacket* pPacket);
   bool sendPacketTo(int destinationID, GenericPacket* pPacket);

protected:

private:

   int subscriber_id_;

   // Generic packet receiver. Can be a queue, hook, etc.
   PacketReceiver* receiver_ptr_;

   mutable boost::mutex subscribe_lock_;
};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_SUBSCRIBER_H


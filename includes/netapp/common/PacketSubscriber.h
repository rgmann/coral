#ifndef PACKET_SUBSCRIBER_H
#define PACKET_SUBSCRIBER_H

namespace liber {
namespace netapp {

class GenericPacket;
class PacketReceiver;

class PacketSubscriber {
public:

   PacketSubscriber();
   virtual ~PacketSubscriber();

   virtual void setReceiver(PacketReceiver* pReceiver);

   virtual void setID(int id);

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

  int mSubscriberId;

  // Generic packet receiver. Can be a queue, hook, etc.
  PacketReceiver* mpReceiver;
};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_SUBSCRIBER_H


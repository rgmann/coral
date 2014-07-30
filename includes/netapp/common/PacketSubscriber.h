#ifndef PACKET_SUBSCRIBER_H
#define PACKET_SUBSCRIBER_H

//#include "GenericPacket.h"

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
   //virtual void processPacket(const GenericPacket* pPacket) = 0;

   /**
    * Add a packet to the end of this PacketApp's input queue (if it has one).
    */
   virtual bool put(const char* pData, ui32 nLength) = 0;

protected:

   virtual bool sendPacket(GenericPacket* pPacket);
   bool sendPacketTo(int destinationID, GenericPacket* pPacket);

private:

   int mSubscriberId;
   // GenericPacket output queue.
   PacketReceiver* mpReceiver;
};

} // End namespace netapp
} // End namespace liber

#endif // PACKET_SUBSCRIBER_H


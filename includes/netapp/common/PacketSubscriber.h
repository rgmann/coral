#ifndef PACKET_SUBSCRIBER_H
#define PACKET_SUBSCRIBER_H

#include <utility>
#include "Queue.h"
#include "GenericPacket.h"
#include "NetAppPacket.h"

namespace liber {
namespace netapp {

class PacketSubscriber {
public:

   PacketSubscriber();
   virtual ~PacketSubscriber();

   typedef std::pair<int, liber::netapp::GenericPacket*> PacketPair;
   void setOutputQueue(Queue<NetAppPacket*>* pQueue);
   void setId(int id);

   /**
    * Synchronously process a packet.  If PacketApp does not perform any
    * synchronous packet processing, simply override with an implementation
    * that does nothing.
    */
   //virtual void processPacket(const GenericPacket* pPacket) = 0;

   static const i32 QueueTimeoutMs = 500;

   /**
    * Add a packet to the end of this PacketApp's input queue (if it has one).
    */
   liber::netapp::GenericPacket* stage();
   void release();

   virtual bool put(const char* pData, ui32 nLength) = 0;

protected:

   //virtual bool recvPacket(GenericPacket** pPacket, i32 nTimeoutMs = QueueTimeoutMs);
   virtual bool sendPacket(liber::netapp::GenericPacket* pPacket, i32 nTimeoutMs = QueueTimeoutMs);

private:

   int mSubscriberId;
   // GenericPacket output queue.
   Queue<NetAppPacket*>* mpOutQueue;
};

}}

#endif // PACKET_SUBSCRIBER_H


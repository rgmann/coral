#ifndef ROUTING_PACKET_H
#define ROUTING_PACKET_H

#include "GenericPacket.h"

class RoutingPacket : public GenericPacket
{
public:

   enum Type {
      // The RoutingPacket does not know about the
      // type of the packet contained within it.
   }

   struct __attribute ((__packed__)) Data {
      static const char marker[] = "liberrouter";
      ui32 length;
   };

   RoutingPacket();
   
   RoutingPacket(const ui8* pData, ui32 nBytes);
   
   /**
    * Get a pointer to the payload data.
    */
   const char* const payloadPtr() const;
   
   /**
    * Get the packet length (no including the header)
    */
   bool getLength(ui32 &nBytes);
   
   bool to(GenericPacket* pPacket);
   
   bool from(GenericPacket* pPacket);

   bool unpack(void* pPkt, ui32 nSizeBytes);

   ui32 dataSize() const;
   
private:
   
   Data* const data();
   
private:

   typedef GenericPacket inherited;

};

#endif // ROUTING_PACKET_H

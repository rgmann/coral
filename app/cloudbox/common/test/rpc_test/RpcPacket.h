#ifndef RPC_PACKET_H
#define RPC_PACKET_H

#include "GenericPacket.h"
#include "Structure.h"

class RpcPacket : public GenericPacket
{
public:

   enum Type
   {
      // TODO: add types
   };

   static const ui32 RpcMarkerSize = 6;
   struct __attribute ((__packed__)) Data
   {
      char marker[RpcMarkerSize];
      ui32 rpcId;
      ui32 length;
      ui32 format;
      ui32 encoding;
   };

   RpcPacket();
   
   RpcPacket(const TransportObject &object, 
             TransportObject::Encoding encoding = TransportObject::Base64);
   
   bool  getObject(TransportObject &object) const;

   bool  unpack(void* pPkt, ui32 nSizeBytes);
   
   Data* const data() const;

private:
   
   ui32  dataSize() const;
   
private:

   typedef GenericPacket inherited;

};

#endif // RPC_PACKET_H

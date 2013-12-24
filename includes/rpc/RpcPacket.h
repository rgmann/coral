#ifndef RPC_PACKET_H
#define RPC_PACKET_H

#include "GenericPacket.h"
#include "RpcObject.h"

namespace liber {
namespace rpc {

class RpcPacket : public liber::netapp::GenericPacket {
public:

   enum Type
   {
      // TODO: add types
   };

   static const ui32 RpcMarkerSize = 7;
   struct __attribute ((__packed__)) Data
   {
      char marker[RpcMarkerSize];
      ui32 rpcId;
      ui32 length;
      ui32 format;
      ui32 encoding;
   };

   RpcPacket();
   
   RpcPacket(const RpcObject &object); 
   
   bool  getObject(RpcObject &object) const;

   bool  unpack(const void* pPkt, ui32 nSizeBytes);
   
   Data* const data() const;

//  virtual void*        dataPtr();
//  virtual void* const  dataPtr() const;

//private:
   
//   ui32  dataSize() const;
   
private:

   typedef liber::netapp::GenericPacket inherited;

};

}}

#endif // RPC_PACKET_H

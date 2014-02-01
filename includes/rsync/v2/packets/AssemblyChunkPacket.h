#ifndef ASSEMBLY_CHUNK_PACKET_H
#define ASSEMBLY_CHUNK_PACKET_H

#include "GenericPacket.h"

namespace liber {
namespace rsync {

class AssemblyChunkPacket : public liber::netapp::GenericPacket {
public:
   
   enum Type
   {
   };
   
   // AssemblyChunkPacket contains a raw, dynamically sized data block.
   struct __attribute__((__packed__)) Data
   {
   };
   
   AssemblyChunkPacket();
   
   /**
    * If type = RsyncChunkType, then nParam is treated as the chunk size
    * and used in allocating the data.
    *
    * If type = RsyncSegmentType, then nParam is treated as the segment ID
    * and the data pointer is NULL.
    */
   AssemblyChunkPacket(ui32 nChunkSizeBytes);
   
   ui8* const data();
   
   bool  unpack(const void* pPkt, ui32 nSizeBytes);
   
private:
   
   typedef liber::netapp::GenericPacket inherited;
};

}}

#endif // ASSEMBLY_CHUNK_PACKET_H

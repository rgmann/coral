#ifndef ASSEMBLY_CHUNK_PACKET_H
#define ASSEMBLY_CHUNK_PACKET_H

#include "GenericPacket.h"

class AssemblyChunkPacket : public GenericPacket
{
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
   
   virtual ui32 dataSize() const;
   
   ui8* const data();
   
   bool  unpack(const void* pPkt, ui32 nSizeBytes);
   
private:
   
   typedef GenericPacket inherited;
};

#endif // ASSEMBLY_CHUNK_PACKET_H
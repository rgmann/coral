#ifndef ASSEMBLY_CHUNK_PACKET_H
#define ASSEMBLY_CHUNK_PACKET_H

#include "RsyncAssemblyInstr.h"

class AssemblyChunkPacket : public RsyncAssemblyInstr
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
   
   ui32  dataSize() const;
   
   void* const data() const;
   
protected:
   
   void* dataPtr();
   
private:
   
   typedef RsyncAssemblyInstr inherited;
};

#endif // ASSEMBLY_CHUNK_PACKET_H
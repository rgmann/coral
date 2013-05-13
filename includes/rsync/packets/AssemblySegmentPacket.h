#ifndef ASSEMBLY_SEGMENT_PACKET_H
#define ASSEMBLY_SEGMENT_PACKET_H

#include "RsyncAssemblyInstr.h"

class AssemblySegmentPacket : public RsyncAssemblyInstr
{
public:
   
   enum Type
   {
   };
   
   // AssemblyChunkPacket contains a raw, dynamically sized data block.
   struct __attribute__((__packed__)) Data
   {
      ui32 segmentId;
   };
   
   AssemblySegmentPacket();
   
   /**
    * If type = RsyncChunkType, then nParam is treated as the chunk size
    * and used in allocating the data.
    *
    * If type = RsyncSegmentType, then nParam is treated as the segment ID
    * and the data pointer is NULL.
    */
   AssemblySegmentPacket(ui32 segmentid);
   
   ui32  getSegmentId() const;
   
   void* const data() const;
   
protected:
   
   void* dataPtr();
   
private:
   
   typedef RsyncAssemblyInstr inherited;
};

#endif // ASSEMBLY_SEGMENT_PACKET_H
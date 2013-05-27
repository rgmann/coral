#ifndef ASSEMBLY_SEGMENT_PACKET_H
#define ASSEMBLY_SEGMENT_PACKET_H

#include "GenericPacket.h"

class AssemblySegmentPacket : public GenericPacket
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
   AssemblySegmentPacket(ui32 nSegmentId);
   
   void  setSegmentId(ui32 nSegmentId);
   
   bool  getSegmentId(ui32 &nSegmentId) const;
   
   bool  unpack(const void* pPkt, ui32 nSizeBytes);
   
protected:
   
   ui32  dataSize() const;
      
   Data* const data() const;
      
private:
   
   typedef GenericPacket inherited;
};

#endif // ASSEMBLY_SEGMENT_PACKET_H
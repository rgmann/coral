#ifndef RSYNC_ASSEMBLY_INSTR_H
#define RSYNC_ASSEMBLY_INSTR_H

#include "RsyncPacket.h"

//struct RsyncAssemblyInstrHdr
//{
//   static const unsigned int marker = 0x1B2CA5A3;
//   unsigned char type;
//   
//   union {
//      unsigned int segmentId;
//      unsigned int chunkSizeBytes;
//      unsigned int relPathLen;
//      unsigned int generic;
//   } info;
//};

class RsyncAssemblyInstr : public RsyncPacket
{
public:
   
   enum Type
   {
      RsyncTypeNotSet,
      MarkerType,
      //RsyncBeginMarker, // Payload is filename
      ChunkType,
      SegmentType//,
      //RsyncEndMarker    // No payload
   };
   
   struct __attribute__((__packed__)) Data
   {
      //static const ui32 Marker = 0x1B2CA5A3;
      ui32 type;
      ui32 length;
   };
   
   RsyncAssemblyInstr();
   
   /**
    * If type = RsyncChunkType, then nParam is treated as the chunk size
    * and used in allocating the data.
    *
    * If type = RsyncSegmentType, then nParam is treated as the segment ID
    * and the data pointer is NULL.
    */
   RsyncAssemblyInstr(RsyncAssmbInstrType type, ui32 nSizeBytes = 0);
   
   ~RsyncAssemblyInstr();
   
   Type type() const;
   
   //RsyncAssemblyInstrHdr* const headerPtr();
   //unsigned int info() const;
   
   //ui32  HeaderSize();
   
   //void* const data() const;
   Data* const data() const;
   
   bool  unpack(void* pPkt, unsigned int nSizeBytes);

protected:
   
   void* dataPtr();
   
   void*       dataEndPtr();
   void* const dataEndPtr() const;
   
private:
   
//   RsyncAssemblyInstrHdr m_Header;
};

#endif // RSYNC_ASSEMBLY_INSTR_H
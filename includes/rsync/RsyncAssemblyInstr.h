#ifndef RSYNC_ASSEMBLY_INSTR_H
#define RSYNC_ASSEMBLY_INSTR_H

#include "GenericPacket.h"

struct RsyncAssemblyInstrHdr
{
   static const unsigned int marker = 0x1B2CA5A3;
   unsigned char type;
   
   union {
      unsigned int segmentId;
      unsigned int chunkSizeBytes;
      unsigned int relPathLen;
      unsigned int generic;
   } info;
};

class RsyncAssemblyInstr : public GenericPacket
{
public:
   
   enum RsyncAssmbInstrType
   {
      RsyncTypeNotSet,
      RsyncBeginMarker, // Payload is filename
      RsyncChunkType,
      RsyncSegmentType,
      RsyncEndMarker    // No payload
   };
   
   RsyncAssemblyInstr();
   
   /**
    * If type = RsyncChunkType, then nParam is treated as the chunk size
    * and used in allocating the data.
    *
    * If type = RsyncSegmentType, then nParam is treated as the segment ID
    * and the data pointer is NULL.
    */
   RsyncAssemblyInstr(RsyncAssmbInstrType type, unsigned int nParam = 0);
   
   ~RsyncAssemblyInstr();
   
   RsyncAssmbInstrType type() const;
   
   //RsyncAssemblyInstrHdr* const headerPtr();
   unsigned int info() const;
   
   bool  unpack(void* pPkt, unsigned int nSizeBytes);
   
private:
   
   RsyncAssemblyInstrHdr m_Header;
};

#endif // RSYNC_ASSEMBLY_INSTR_H
#ifndef RSYNC_ASSEMBLY_INSTR_H
#define RSYNC_ASSEMBLY_INSTR_H

//#include "GenericPacket.h"
#include "AssemblyBeginMarker.h"
#include "AssemblyEndMarker.h"
#include "AssemblyChunkPacket.h"
#include "AssemblySegmentPacket.h"

class RsyncAssemblyInstr : public GenericPacket
{
public:
   
   enum Type
   {
      TypeNotSet,
      BeginMarkerType,
      EndMarkerType,
      ChunkType,
      SegmentType
   };
   
   struct __attribute__((__packed__)) Data
   {
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
   RsyncAssemblyInstr(const AssemblyBeginMarker*    pPacket);
   RsyncAssemblyInstr(const AssemblyEndMarker*      pPacket);
   RsyncAssemblyInstr(const AssemblyChunkPacket*    pPacket);
   RsyncAssemblyInstr(const AssemblySegmentPacket*  pPacket);
   
   ~RsyncAssemblyInstr();
   
   Type type() const;
   
   Data* const data() const;
   
   virtual ui32 dataSize() const;
   
   bool to(AssemblyBeginMarker**    pPacket) const;
   bool to(AssemblyEndMarker**      pPacket) const;
   bool to(AssemblyChunkPacket**    pPacket) const;
   bool to(AssemblySegmentPacket**  pPacket) const;
   
//   bool from(const AssemblyBeginMarker*   pPacket);
//   bool from(const AssemblyEndMarker*     pPacket);
//   bool from(const AssemblyChunkPacket*   pPacket);
//   bool from(const AssemblySegmentPacket* pPacket);
   
   bool  unpack(const void* pPkt, ui32 nSizeBytes);

protected:
   
   bool from(Type type, const GenericPacket* pPacket);
   
   GenericPacket* create(Type type) const;
   
private:
   
   typedef GenericPacket inherited;
};

#endif // RSYNC_ASSEMBLY_INSTR_H
#ifndef  RSYNC_PACKET_H
#define  RSYNC_PACKET_H

#include <stdio.h>
//#include "GenericPacket.h"
#include "RsyncAssemblyInstr.h"
#include "RsyncSegmentReportPacket.h"

class RsyncPacket : public GenericPacket
{
public:
   
   enum Type
   {
      TypeNotSet,
      SegmentReportType,
      AssemblyInstType,
      NumTypes
   };
   
   struct __attribute__((__packed__)) Data
   {
      ui32  marker;
      ui32  type;
      ui32  length;
   };
   static const ui32 RsyncPacketMarker = 0xFEEDBEEF;
   
   RsyncPacket();
   
   RsyncPacket(Type type, ui32 nDataSize);
   
   RsyncPacket(const RsyncAssemblyInstr* pPacket);
   
   RsyncPacket(const RsyncSegmentReportPacket* pPacket);
      
   Type type() const;
   
   bool length(ui32 &length) const;
   
   Data* const data() const;
   
   virtual ui32 dataSize() const;
   
   bool to(RsyncAssemblyInstr** pPacket) const;
   bool to(RsyncSegmentReportPacket** pPacket) const;

   virtual bool unpack(const void* pPkt, ui32 nSizeBytes);

protected:
   
   GenericPacket* create(Type type) const;
   
   bool from(Type type, const GenericPacket* pPacket);
   
private:
   
   typedef GenericPacket inherited;
};

#endif   // RSYNC_PACKET_H
#ifndef RSYNC_SEGMENT_REPORT_PACKET_H
#define RSYNC_SEGMENT_REPORT_PACKET_H

#include "GenericPacket.h"
#include "RsyncSegmentReportHdr.h"
#include "RsyncSegmentPacket.h"

class RsyncSegmentReportPacket : public GenericPacket
{
public:
   
   enum Type
   {
      TypeNotSet,
      HeaderType,
      SegmentType
   };
   
   struct __attribute__((__packed__)) Data
   {
      ui32  type;
      ui32  length;
   };
   
   RsyncSegmentReportPacket();
   RsyncSegmentReportPacket(Type type, ui32 nDataSize);
   RsyncSegmentReportPacket(const RsyncSegmentReportHdr* pPacket);
   RsyncSegmentReportPacket(const RsyncSegmentPacket* pPacket);
      
   virtual ui32 dataSize() const;
   
   ui32 length() const;
   
   Type type() const;
   
   bool to(RsyncSegmentReportHdr** pPacket);
   bool to(RsyncSegmentPacket** pPacket);
      
   virtual bool unpack(const void* pPkt, ui32 nSizeBytes);

protected:

   bool from(Type type, const GenericPacket* pPacket);
   
   GenericPacket* create(Type type) const;

private:
   
   typedef GenericPacket inherited;
   
   Data* data();

   Data* const data() const;
};

#endif // RSYNC_SEGMENT_REPORT_PACKET_H
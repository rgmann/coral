#ifndef RSYNC_SEGMENT_REPORT_PACKET_H
#define RSYNC_SEGMENT_REPORT_PACKET_H

#include "RsyncPacket.h"

//struct __attribute__((__packed__)) RsyncSegmentReportPacketHdr
//{
//   //static const ui32 marker = 0x00000000;
//   ui32  type;
//   ui32  length;
//};

class RsyncSegmentReportPacket : public RsyncPacket
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
   
   //virtual ~RsyncSegmentReportPacket();
   
   Type type() const;
   
   //virtual void* const data() const;
   
   //virtual bool   unpack(const void* pPkt, unsigned int nSizeBytes);

protected:
   
   //virtual void* dataPtr();
   
private:
   
   Data* headerData();

   const Data* headerData() const;
};

#endif // RSYNC_SEGMENT_REPORT_PACKET_H
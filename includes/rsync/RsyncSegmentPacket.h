#ifndef RSYNC_SEGMENT_PACKET_H
#define RSYNC_SEGMENT_PACKET_H

#include <string>
#include "RsyncSegmentReportPacket.h"
#include "Md5Hash.h"
#include "RsyncCommon.h"

//struct __attribute__((__packed__)) RsyncSegmentData
//{
//   ui32     segmentId;
//   i32      weakChecksum;
//   Hash128  strongChecksum;
//   ui16     segmentSizeBytes;
//};

class RsyncSegmentPacket : public RsyncSegmentReportPacket
{
public:
   
   enum Type {};
   
   struct __attribute__((__packed__)) Data
   {
      ui32     segmentId;
      i32      weakChecksum;
      Hash128  strongChecksum;
      ui16     segmentSizeBytes;
   };
   
   RsyncSegmentPacket();
   
   //~RsyncSegmentPacket();
   
   bool  setSegmentId(ui32 segmentId);
   bool  getSegmentId(ui32 &segmentId) const;
   
   bool  setWeak(i32 weakChecksum);
   bool  getWeak(i32 &weakChecksum) const;
   
   bool  setStrong(Hash128 strongChecksum);
   bool  getStrong(Hash128 &strongChecksum) const;
   
   bool  setSegmentSize(ui16 segmentSizeBytes);
   bool  getSegmentSize(ui16 &segmentSizeBytes) const;
   
private:
   
//   RsyncSegmentData* segmentData();
//   const RsyncSegmentData* segmentData() const;
   Data* segmentData();
   const Data* segmentData() const;
};

#endif // RSYNC_SEGMENT_PACKET_H
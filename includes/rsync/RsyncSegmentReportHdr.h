#ifndef RSYNC_SEGMENT_REPORT_HDR_H
#define RSYNC_SEGMENT_REPORT_HDR_H

#include <string>
#include "RsyncCommon.h"
#include "RsyncSegmentReportPacket.h"

//struct __attribute__((__packed__)) RsyncSegmentReportHdrData
//{
//   char  filePath[RsyncMaxPathLength];
//   ui32  segmentCount;
//   ui32  segmentSizeBytes;
//};

class RsyncSegmentReportHdr : public RsyncSegmentReportPacket
{
public:
   
   enum Type {};
   
   struct __attribute__((__packed__)) Data
   {
      char  filePath[RsyncMaxPathLength];
      ui32  segmentCount;
      ui32  segmentSizeBytes;
   };
   
   RsyncSegmentReportHdr();
   
   //~RsyncSegmentReportHdr();
   
   bool setFilePath(std::string filePath);
   bool getFilePath(std::string &filePath) const;

   bool setSegmentCount(ui32 segmentCount);
   bool getSegmentCount(ui32 &segmentCount) const;
   
   bool setSegmentSize(ui32 segmentSizeBytes);
   bool getSegmentSize(ui32 &segmentSizeBytes) const;
   
private:
   
//   RsyncSegmentReportHdrData* headerData();
//   const RsyncSegmentReportHdrData* headerData() const;
   Data* headerData();
   const Data* headerData() const;
};

#endif // RSYNC_SEGMENT_REPORT_HDR_H
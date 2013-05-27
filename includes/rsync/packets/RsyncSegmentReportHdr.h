#ifndef RSYNC_SEGMENT_REPORT_HDR_H
#define RSYNC_SEGMENT_REPORT_HDR_H

#include <string>
#include "RsyncCommon.h"
#include "GenericPacket.h"


class RsyncSegmentReportHdr : public GenericPacket
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
   
   ~RsyncSegmentReportHdr();
   
   bool setFilePath(std::string filePath);
   bool getFilePath(std::string &filePath) const;

   bool setSegmentCount(ui32 segmentCount);
   bool getSegmentCount(ui32 &segmentCount) const;
   
   bool setSegmentSize(ui32 segmentSizeBytes);
   bool getSegmentSize(ui32 &segmentSizeBytes) const;
   
protected:
   
private:
   
   typedef GenericPacket inherited;
   
   Data* headerData();
   Data* const headerData() const;
};

#endif // RSYNC_SEGMENT_REPORT_HDR_H
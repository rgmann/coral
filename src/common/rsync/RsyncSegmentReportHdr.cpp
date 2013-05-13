#include <string.h>
#include "RsyncSegmentReportHdr.h"

//------------------------------------------------------------------------------
RsyncSegmentReportHdr::RsyncSegmentReportHdr()
 : RsyncSegmentReportPacket(HeaderType,
                            sizeof(Data))
//                            sizeof(RsyncSegmentReportHdrData))
{
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::setFilePath(std::string filePath)
{
   if (!headerData()) return false;
   
   strncpy(headerData()->filePath, filePath.c_str(), RsyncMaxPathLength);
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::getFilePath(std::string &filePath) const
{
   if (!headerData()) return false;
   
   filePath = std::string(headerData()->filePath);
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::setSegmentCount(ui32 segmentCount)
{
   if (!headerData()) return false;
   
   headerData()->segmentCount = segmentCount;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::getSegmentCount(ui32 &segmentCount) const
{
   if (!headerData()) return false;
   
   segmentCount = headerData()->segmentCount;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::setSegmentSize(ui32 segmentSizeBytes)
{
   if (!headerData()) return false;
   
   headerData()->segmentSizeBytes = segmentSizeBytes;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::getSegmentSize(ui32 &segmentSizeBytes) const
{
   if (!headerData()) return false;
   
   segmentSizeBytes = headerData()->segmentSizeBytes;
   
   return true;
}

//------------------------------------------------------------------------------
//RsyncSegmentReportHdrData* RsyncSegmentReportHdr::headerData()
RsyncSegmentReportHdr::Data* RsyncSegmentReportHdr::headerData()
{
//   return reinterpret_cast<RsyncSegmentReportHdrData*>(dataPtr());
   return reinterpret_cast<Data*>(dataPtr());
}

//------------------------------------------------------------------------------
//const RsyncSegmentReportHdrData* RsyncSegmentReportHdr::headerData() const
const RsyncSegmentReportHdr::Data* RsyncSegmentReportHdr::headerData() const
{
//   return reinterpret_cast<const RsyncSegmentReportHdrData*>(data());
   return reinterpret_cast<const Data*>(data());
}

#include <string.h>
#include "RsyncSegmentReportHdr.h"

//------------------------------------------------------------------------------
RsyncSegmentReportHdr::RsyncSegmentReportHdr()
 : GenericPacket(sizeof(Data))
{
}

//------------------------------------------------------------------------------
RsyncSegmentReportHdr::~RsyncSegmentReportHdr()
{
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::setFilePath(std::string filePath)
{
   if (!isAllocated()) return false;
   
   strncpy(headerData()->filePath, filePath.c_str(), RsyncMaxPathLength);
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::getFilePath(std::string &filePath) const
{
   if (!isAllocated()) return false;
   
   filePath = std::string(headerData()->filePath);
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::setSegmentCount(ui32 segmentCount)
{
   if (!isAllocated()) return false;
   
   headerData()->segmentCount = segmentCount;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::getSegmentCount(ui32 &segmentCount) const
{
   if (!isAllocated()) return false;
   
   segmentCount = headerData()->segmentCount;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::setSegmentSize(ui32 segmentSizeBytes)
{
   if (!isAllocated()) return false;
   
   headerData()->segmentSizeBytes = segmentSizeBytes;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentReportHdr::getSegmentSize(ui32 &segmentSizeBytes) const
{
   if (!isAllocated()) return false;
   
   segmentSizeBytes = headerData()->segmentSizeBytes;
   
   return true;
}

//------------------------------------------------------------------------------
RsyncSegmentReportHdr::Data* RsyncSegmentReportHdr::headerData()
{
   return reinterpret_cast<Data*>(basePtr());
}

//------------------------------------------------------------------------------
RsyncSegmentReportHdr::Data* const RsyncSegmentReportHdr::headerData() const
{
   return reinterpret_cast<Data* const>(basePtr());
}

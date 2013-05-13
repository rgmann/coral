#include <string.h>
#include "RsyncSegmentPacket.h"

//------------------------------------------------------------------------------
RsyncSegmentPacket::RsyncSegmentPacket()
//: RsyncSegmentReportPacket(SegmentType, sizeof(RsyncSegmentData))
: RsyncSegmentReportPacket(SegmentType, sizeof(Data))
{
   if (dataPtr())
   {
//      memset(dataPtr(), 0, sizeof(RsyncSegmentData));
      memset(dataPtr(), 0, sizeof(Data));
   }
}


//------------------------------------------------------------------------------
bool RsyncSegmentPacket::setSegmentId(ui32 segmentId)
{
   if (!segmentData()) return false;
   
   segmentData()->segmentId = segmentId;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentPacket::getSegmentId(ui32 &segmentId) const
{
   if (!segmentData()) return false;
   
   segmentId = segmentData()->segmentId;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentPacket::setWeak(i32 weakChecksum)
{
   if (!segmentData()) return false;
   
   segmentData()->weakChecksum = weakChecksum;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentPacket::getWeak(i32 &weakChecksum) const
{
   if (!segmentData()) return false;
   
   weakChecksum = segmentData()->weakChecksum;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentPacket::setStrong(Hash128 strongChecksum)
{
   if (!segmentData()) return false;
   
   segmentData()->strongChecksum = strongChecksum;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentPacket::getStrong(Hash128 &strongChecksum) const
{
   if (!segmentData()) return false;
   
   strongChecksum = segmentData()->strongChecksum;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentPacket::setSegmentSize(ui16 segmentSizeBytes)
{
   if (!segmentData()) return false;
   
   segmentData()->segmentSizeBytes = segmentSizeBytes;
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncSegmentPacket::getSegmentSize(ui16 &segmentSizeBytes) const
{
   if (!segmentData()) return false;
   
   segmentSizeBytes = segmentData()->segmentSizeBytes;
   
   return true;
}

//------------------------------------------------------------------------------
//RsyncSegmentData* RsyncSegmentPacket::segmentData()
RsyncSegmentPacket::Data* RsyncSegmentPacket::segmentData()
{
//   return reinterpret_cast<RsyncSegmentData*>(dataPtr());
   return reinterpret_cast<Data*>(dataPtr());
}

//------------------------------------------------------------------------------
//const RsyncSegmentData* RsyncSegmentPacket::segmentData() const
const RsyncSegmentPacket::Data* RsyncSegmentPacket::segmentData() const
{
//   return reinterpret_cast<const RsyncSegmentData*>(data());
   return reinterpret_cast<const Data*>(data());
}

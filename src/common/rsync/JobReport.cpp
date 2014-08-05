#include "Log.h"
#include "JobReport.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

void serializeTS(SerialStream& ctor, const Timestamp& ts)
{
  ui32 temp = ts.seconds();
  ctor.write(temp);
  temp = ts.nanoseconds();
  ctor.write(temp);
}

bool deserializeTS(SerialStream& dtor, Timestamp& ts)
{
  ui32 temp = 0;
  if (dtor.read(temp) == false)
  {
    log::error("deserializeTS - Failed deserialize seconds\n");
    return false;
  }
  ts.seconds((time_t)temp);

  if (dtor.read(temp) == false)
  {
    log::error("deserializeTS - Failed deserialize seconds\n");
    return false;
  }
  ts.nanoseconds(temp);

  return true;
}

//----------------------------------------------------------------------------
SegmentationReport::SegmentationReport()
: Serializable()
, status(RsyncSuccess)
, segmentSizeBytes(0)
, strideSizeBytes(0)
, segmentCount(0)
{
}

//----------------------------------------------------------------------------
void SegmentationReport::print(std::ostream& stream, ui8 nSpaces) const
{
  char* pSpaces = new char[nSpaces + 1];
  memset(pSpaces, ' ', nSpaces);
  pSpaces[nSpaces] = '\0';
  stream << pSpaces << "segment size (bytes): " << segmentSizeBytes << std::endl; 
  stream << pSpaces << "stride size (bytes): " << strideSizeBytes << std::endl; 
  stream << pSpaces << "segment count: " << segmentCount << std::endl; 
  stream << pSpaces << "start time: " << std::fixed << begin.fseconds() << std::endl; 
  stream << pSpaces << "end time: " << std::fixed << end.fseconds() << std::endl; 
  delete[] pSpaces;
}

//----------------------------------------------------------------------------
void SegmentationReport::pack(liber::netapp::SerialStream& ctor)
{
  const_cast<const SegmentationReport*>(this)->pack(ctor);
}

//----------------------------------------------------------------------------
void SegmentationReport::pack(liber::netapp::SerialStream& ctor) const
{
  ctor.write((ui32)status);
  ctor.write(segmentSizeBytes);
  ctor.write(strideSizeBytes);
  ctor.write(segmentCount);
  serializeTS(ctor, begin);
  serializeTS(ctor, end);
}

//----------------------------------------------------------------------------
bool SegmentationReport::unpack(SerialStream& dtor)
{
  ui32 tempStatus = 0;
  if (!dtor.read(tempStatus))
  {
    log::error("SegmentationReport::unpack - Failed to read status\n");
    return false;
  }
  status = (RsyncError)tempStatus;

  if (!dtor.read(segmentSizeBytes))
  {
    log::error("SegmentationReport::unpack - Failed to read rcv segment size\n");
    return false;
  }

  if (!dtor.read(strideSizeBytes))
  {
    log::error("SegmentationReport::unpack - Failed to read stride size\n");
    return false;
  }

  if (!dtor.read(segmentCount))
  {
    log::error("SegmentationReport::unpack - Failed to read segment count\n");
    return false;
  }

  if (!deserializeTS(dtor, begin))
  {
    log::error("SegmentationReport::unpack - Failed to read hashBegin.mSeconds\n");
    return false;
  }

  if (!deserializeTS(dtor, end))
  {
    log::error("SegmentationReport::unpack - Failed to read hashEnd.mSeconds\n");
    return false;
  }

  return true;
}
//----------------------------------------------------------------------------
AssemblyReport::AssemblyReport()
: Serializable()
, status(RsyncSuccess)
, segmentCount(0)
, chunkCount(0)
{
}

//----------------------------------------------------------------------------
void AssemblyReport::print(std::ostream& stream, ui8 nSpaces) const
{
  char* pSpaces = new char[nSpaces + 1];
  memset(pSpaces, ' ', nSpaces);
  pSpaces[nSpaces] = '\0';
  stream << pSpaces << "segment count: " << segmentCount << std::endl; 
  stream << pSpaces << "chunk count: " << chunkCount << std::endl; 
  stream << pSpaces << "start time: " << std::fixed << begin.fseconds() << std::endl; 
  stream << pSpaces << "end time: " << std::fixed << end.fseconds() << std::endl; 
  delete[] pSpaces;
}

//----------------------------------------------------------------------------
void AssemblyReport::pack(SerialStream& ctor)
{
  const_cast<const AssemblyReport*>(this)->pack(ctor);
}

//----------------------------------------------------------------------------
void AssemblyReport::pack(SerialStream& ctor) const
{
  ctor.write((ui32)status);
  ctor.write(segmentCount);
  ctor.write(chunkCount);
  serializeTS(ctor, begin);
  serializeTS(ctor, end);
}

//----------------------------------------------------------------------------
bool AssemblyReport::unpack(SerialStream& dtor)
{
  ui32 tempStatus = 0;
  if (!dtor.read(tempStatus))
  {
    log::error("AssemblyReport::unpack - Failed to read status\n");
    return false;
  }
  status = (RsyncError)tempStatus;

  if (!dtor.read(segmentCount))
  {
    log::error("AssemblyReport::unpack - Failed to read rcv segment count\n");
    return false;
  }

  if (!dtor.read(chunkCount))
  {
    log::error("AssemblyReport::unpack - Failed to read chunk count\n");
    return false;
  }

  if (!deserializeTS(dtor, begin))
  {
    log::error("AssemblyReport::unpack - Failed to read hashBegin.mSeconds\n");
    return false;
  }

  if (!deserializeTS(dtor, end))
  {
    log::error("AssemblyReport::unpack - Failed to read hashEnd.mSeconds\n");
    return false;
  }

  return true;
}


//----------------------------------------------------------------------------
AuthorityReport::AuthorityReport()
: Serializable()
, status(RsyncSuccess)
, receivedSegmentCount(0)
, matchedSegmentCount(0)
, chunkCount(0)
{
}

//----------------------------------------------------------------------------
void AuthorityReport::print(std::ostream& stream, ui8 nSpaces) const
{
  char* pSpaces = new char[nSpaces + 1];
  memset(pSpaces, ' ', nSpaces);
  pSpaces[nSpaces] = '\0';
  stream << pSpaces << "segments received: " << receivedSegmentCount << std::endl; 
  stream << pSpaces << "segments matched: " << matchedSegmentCount << std::endl; 
  stream << pSpaces << "chunks sent: " << chunkCount << std::endl; 
  stream << pSpaces << "hash start time: " << std::fixed << hashBegin.fseconds() << std::endl; 
  stream << pSpaces << "hash end time: " << std::fixed << hashEnd.fseconds() << std::endl; 
  stream << pSpaces << "auth start time: " << std::fixed << authBegin.fseconds() << std::endl; 
  stream << pSpaces << "auth end time: " << std::fixed << authEnd.fseconds() << std::endl; 
  delete[] pSpaces;
}

//----------------------------------------------------------------------------
void AuthorityReport::pack(SerialStream& ctor)
{
  const_cast<const AuthorityReport*>(this)->pack(ctor);
}

//----------------------------------------------------------------------------
void AuthorityReport::pack(SerialStream& ctor) const
{
  ctor.write((ui32)status);
  ctor.write(receivedSegmentCount);
  ctor.write(matchedSegmentCount);
  ctor.write(chunkCount);
  serializeTS(ctor, hashBegin);
  serializeTS(ctor, hashEnd);
  serializeTS(ctor, authBegin);
  serializeTS(ctor, authEnd);
}

//----------------------------------------------------------------------------
bool AuthorityReport::unpack(SerialStream& dtor)
{
  ui32 tempStatus = 0;
  if (dtor.read(tempStatus) == false)
  {
    log::error("AuthorityReport::unpack - Failed to read status\n");
    return false;
  }
  status = (RsyncError)tempStatus;

  if (dtor.read(receivedSegmentCount) == false)
  {
    log::error("AuthorityReport::unpack - Failed to read rcv segment count\n");
    return false;
  }

  if (dtor.read(matchedSegmentCount) == false)
  {
    log::error("AuthorityReport::unpack - Failed to read match segment count\n");
    return false;
  }

  if (!dtor.read(chunkCount))
  {
    log::error("AuthorityReport::unpack - Failed to read chunk count\n");
    return false;
  }

  if (deserializeTS(dtor, hashBegin) == false)
  {
    log::error("AuthorityReport::unpack - Failed to read hashBegin\n");
    return false;
  }

  if (deserializeTS(dtor, hashEnd) == false)
  {
    log::error("AuthorityReport::unpack - Failed to read hashEnd\n");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
void DestinationReport::pack(SerialStream& ctor)
{
  const_cast<const DestinationReport*>(this)->pack(ctor);
}

//----------------------------------------------------------------------------
void DestinationReport::pack(liber::netapp::SerialStream& ctor) const
{
  segmentation.serialize(ctor);
  assembly.serialize(ctor);
}

//----------------------------------------------------------------------------
bool DestinationReport::unpack(liber::netapp::SerialStream& dtor)
{
  if (segmentation.deserialize(dtor) == false)
  {
    log::error("DestinationReport::deserialize - Failed to deserialize dest seg report\n");
    return false;
  }

  if (assembly.deserialize(dtor) == false)
  {
    log::error("JobReport::deserialize - Failed to deserialize dest assembly report\n");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
void SourceReport::pack(SerialStream& ctor)
{
  const_cast<const SourceReport*>(this)->pack(ctor);
}

//----------------------------------------------------------------------------
void SourceReport::pack(liber::netapp::SerialStream& ctor) const
{
  segmentation.serialize(ctor);
  authority.serialize(ctor);
}

//----------------------------------------------------------------------------
bool SourceReport::unpack(liber::netapp::SerialStream& dtor)
{
  if (segmentation.deserialize(dtor) == false)
  {
    log::error("SourceReport::deserialize - Failed to deserialize source seg report\n");
    return false;
  }

  if (authority.deserialize(dtor) == false)
  {
    log::error("SourceReport::deserialize - Failed to deserialize source auth report\n");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------
JobReport::JobReport()
: Serializable()
{
}

//----------------------------------------------------------------------------
void JobReport::print(std::ostream& stream) const
{
  stream << "RSYNC Job Report:" << std::endl
         << " Destination Segmentation Report:" << std::endl;
  destination.segmentation.print(stream, 2);
  stream << " Source Segmentation Report:" << std::endl;
  source.segmentation.print(stream, 2);
  stream << " Source Authority Report:" << std::endl;
  source.authority.print(stream, 2);
  stream << " Destination Assembly Report:" << std::endl;
  destination.assembly.print(stream, 2);
}

//----------------------------------------------------------------------------
void JobReport::pack(SerialStream& ctor)
{
  const_cast<const JobReport*>(this)->pack(ctor);
}

//----------------------------------------------------------------------------
void JobReport::pack(liber::netapp::SerialStream& ctor) const
{
  destination.segmentation.serialize(ctor);
  destination.assembly.serialize(ctor);
  source.segmentation.serialize(ctor);
  source.authority.serialize(ctor);
}

//----------------------------------------------------------------------------
bool JobReport::unpack(liber::netapp::SerialStream& dtor)
{
  if (destination.deserialize(dtor) == false)
  {
    log::error("JobReport::deserialize - Failed to deserialize dest report\n");
    return false;
  }

  if (source.deserialize(dtor) == false)
  {
    log::error("JobReport::deserialize - Failed to deserialize source report\n");
    return false;
  }

  return true;
}



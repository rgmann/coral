#include "JobReport.h"

using namespace liber::rsync;

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
JobReport::JobReport()
{
}

//----------------------------------------------------------------------------
void JobReport::print(std::ostream& stream) const
{
  stream << "RSYNC Job Report:" << std::endl
         << " Destination Segmentation Report:" << std::endl;
  mDestinationReport.segmentation.print(stream, 2);
  stream << " Source Segmentation Report:" << std::endl;
  mSourceReport.segmentation.print(stream, 2);
  stream << " Source Authority Report:" << std::endl;
  mSourceReport.authority.print(stream, 2);
  stream << " Destination Assembly Report:" << std::endl;
  mDestinationReport.assembly.print(stream, 2);
}


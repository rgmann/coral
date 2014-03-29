#include <stdio.h>
#include "Segment.h"
#include "Segmenter.h"
#include "CircularBuffer.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JobDescriptor::JobDescriptor()
{
}

//-----------------------------------------------------------------------------
JobDescriptor::
JobDescriptor(const boost::filesystem::path& path, ui32 nSegmentSizeBytes)
: mnSegmentSizeBytes(nSegmentSizeBytes)
, mPath(path)
{
}

//-----------------------------------------------------------------------------
ui32 JobDescriptor::getSegmentSize() const
{
  return mnSegmentSizeBytes;
}

//-----------------------------------------------------------------------------
boost::filesystem::path& JobDescriptor::getPath()
{
  return mPath;
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getPath() const
{
  return mPath;
}


//-----------------------------------------------------------------------------
// Class: Segmenter
//-----------------------------------------------------------------------------
Segmenter::Segmenter()
{
}

//-----------------------------------------------------------------------------
Segmenter::Segmenter(ui32 nSegmentSizeBytes, SegmentDistance distance)
: mnSegmentSizeBytes(nSegmentSizeBytes)
, mDistance(distance)
{
}

//-----------------------------------------------------------------------------
bool Segmenter::process(std::istream& istream, SegmentReceiver& rReceiver)
{
  ui8*  lpFileBuffer = NULL;
  ui8*  lpReadBuffer = NULL;
  ui8*  lpTempBuffer = NULL;
//  char   lcTempChar;
  Segment::ID lnSegId = 0;

  // Offset, in bytes, from the beginning of the file.
  ui32 lnOffset = 0;

  Adler32Checksum   oldweaksum;
  Md5Hash           tempStrongHash;
  CircularBuffer    lCircularBuff;

  if (istream.bad())
  {
    return false;
  }

  // Allocated the buffer to the segment length.
  if (!lCircularBuff.allocate(mnSegmentSizeBytes))
  {
    return false;
  }

  lpFileBuffer = new ui8[mnSegmentSizeBytes];
  if (lpFileBuffer == NULL)
  {
    return false;
  }

  lpTempBuffer = new ui8[mnSegmentSizeBytes];
  if (lpTempBuffer == NULL)
  {
    return false;
  }


  lpReadBuffer = new ui8[mnSegmentSizeBytes];
  if (lpReadBuffer == NULL)
  {
    delete[] lpFileBuffer;
    lpFileBuffer = NULL;
    return false;
  }

  ui32 lnStrideSizeBytes = (mDistance == FullStride) ? mnSegmentSizeBytes : 1;

  while ((istream.eof() == false) || (lCircularBuff.isEmpty() == false))
  {
    if ((lnOffset % lnStrideSizeBytes) == 0)
    {
      ui32 lnReadLen = 0;
      Adler32Checksum* pAdler = NULL;

      // Compute the number of bytes that should be read.
      // If the stride is one, the first read will read the full segment size, but all
      // following reads will read only one byte.
      lnReadLen = mnSegmentSizeBytes;
      if ((mDistance == EveryOffset) && (lnOffset > 0))
      {
        pAdler = &oldweaksum;
        lnReadLen = 1;
      }
      //printf("read_len=%u, pAdler=%p\n", lnReadLen, pAdler);

      // Read the data into the buffer.
      if (istream.eof() == false)
      {
        istream.read((char*)lpFileBuffer, lnReadLen);
        lCircularBuff.write((char*)lpFileBuffer, istream.gcount());
#ifdef DEBUG_SEGMENTER
        printf("bytes_read=%d\n", (int)istream.gcount());
#endif
      }

      ui32 lnSegmentLength = lCircularBuff.peek((char*)lpReadBuffer, mnSegmentSizeBytes);
#ifdef DEBUG_SEGMENTER
      printf("segment_length=%u\n", lnSegmentLength);
#endif

      // If the stride is the full segment, drain the full segment from the
      // buffer.  Drain one byte if we're striding to each offset.
      lCircularBuff.read((char*)lpTempBuffer,
                         (mDistance == FullStride) ? mnSegmentSizeBytes : 1);

      // Create the new segment.  The segment will compute its weak checksum.
      // The Segment always defers its MD5 computation.
      Segment* lpSegment = new Segment(lnSegId++, lnOffset);
      //lpSegment->setData(lpReadBuffer, lnSegmentLength, pAdler);
      lpSegment->setData(lpReadBuffer, mnSegmentSizeBytes, lnSegmentLength, pAdler);

      // Get the weak checksum for the next iteration just in case
      // checksum rolling is enabled.
      oldweaksum = lpSegment->getWeak();

      // Add the segment to the segment list vector.
      rReceiver.push(lpSegment);
    }

    lnOffset += 1;
  }

  delete[] lpFileBuffer;
  delete[] lpReadBuffer;
  delete[] lpTempBuffer;

  return true;
}


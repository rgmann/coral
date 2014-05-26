#include <stdio.h>
#include "Log.h"
#include "CircularBuffer.h"
#include "Segment.h"
#include "SegmentReceiver.h"
#include "SegmentHook.h"
#include "JobReport.h"
#include "Segmenter.h"

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
// Class: Segmenter
//-----------------------------------------------------------------------------
Segmenter::Segmenter(Segmenter::SegmentDistance distance)
: mDistance(distance)
{
}

//-----------------------------------------------------------------------------
bool Segmenter::
process(std::istream& istream, SegmentReceiver& rReceiver, ui32 nSegmentSizeBytes, SegmentationReport& rReport)
{
  bool lbSuccess = false;

  if (mDistance == FullStride)
  {
    lbSuccess = processFullStride(istream, rReceiver, nSegmentSizeBytes, rReport);
  }
  else
  {
    lbSuccess = processEveryOffset(istream, rReceiver, nSegmentSizeBytes, rReport);
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool Segmenter::
processFullStride(std::istream& istream, SegmentReceiver& rReceiver, ui32 nSegmentSizeBytes, SegmentationReport& rReport)
{
  Segment::ID lnSegId = 0;
  Segment lSegment(nSegmentSizeBytes);

  // Offset, in bytes, from the beginning of the file.
  ui32 lnOffset = 0;

  if (istream.bad())
  {
    return false;
  }

  // Capture the start time.
  rReport.segmentCount = 0;
  rReport.begin.sample();

  ui32 lnStrideSizeBytes = nSegmentSizeBytes;

  // Record the stride size and segment size;
  rReport.strideSizeBytes  = lnStrideSizeBytes;
  rReport.segmentSizeBytes = nSegmentSizeBytes;

  while (istream.eof() == false)
  {
    // Create the new segment.  The segment will compute its weak checksum.
    // The Segment always defers its MD5 computation.
    lSegment.setID(lnSegId++);
    lSegment.setOffset(lnOffset);
    lSegment.setData(istream, nSegmentSizeBytes, NULL);
 
    // Add the segment to the segment list vector.
    rReceiver.push(lSegment);
    rReport.segmentCount++;

    // Move to the next byte.
    lnOffset += lnStrideSizeBytes;
  }

  // Push an empty segment to mark the end of the stream.
  lSegment.setID(Segment::EndOfStream);
  rReceiver.push(lSegment);

  // Capture the segmentation end time.
  rReport.end.sample();

  return true;
}

//-----------------------------------------------------------------------------
bool Segmenter::
processEveryOffset(std::istream& istream, SegmentReceiver& rReceiver, ui32 nSegmentSizeBytes, SegmentationReport& rReport)
{
  ui8*  lpTempBuffer = NULL;

  Segment::ID lnSegId = 0;

  Segment lSegment(nSegmentSizeBytes);

  // Offset, in bytes, from the beginning of the file.
  ui32 lnOffset = 0;

  Adler32Checksum   oldweaksum;
  CircularBuffer    lCircularBuff;

  if (istream.bad())
  {
    return false;
  }

  // Allocated the buffer to the segment length.
  if (!lCircularBuff.allocate(nSegmentSizeBytes))
  {
    return false;
  }

  lpTempBuffer = new ui8[nSegmentSizeBytes];
  if (lpTempBuffer == NULL)
  {
    return false;
  }

  // Capture the start time.
  rReport.segmentCount = 0;
  rReport.begin.sample();

  // Record the stride size and segment size;
  rReport.strideSizeBytes  = 1;
  rReport.segmentSizeBytes = nSegmentSizeBytes;

  //ui32 lnReadLen = nSegmentSizeBytes;
  while ((istream.eof() == false) || (lCircularBuff.isEmpty() == false))
  {
    //ui32 lnReadLen = nSegmentSizeBytes;

    // Compute the number of bytes that should be read.
    // If the stride is one, the first read will read the full segment size, but all
    // following reads will read only one byte.
    //if (lnOffset > 0)
    //{
    //  lnReadLen = 1;
    //}

    // Read the data into the buffer.
    if (istream.eof() == false)
    {
      //lCircularBuff.write(istream, lnReadLen);
      lCircularBuff.write(istream, (lnOffset == 0) ? nSegmentSizeBytes : 1);
    }

    // Create the new segment.  The segment will compute its weak checksum.
    // The Segment always defers its MD5 computation.
    //printf("every_offset: ID=%d, seg_size=%d, seg_len=%d, d[0]=%02X\n",
    //           lnSegId, nSegmentSizeBytes, lnSegmentLength, lpReadBuffer[0]);
    lSegment.setID(lnSegId++);
    lSegment.setOffset(lnOffset);
    lSegment.setData(lCircularBuff,
                     nSegmentSizeBytes,
                     (lnOffset > 0) ? &oldweaksum : NULL);

    // Get the weak checksum for the next iteration just in case
    // checksum rolling is enabled.
    oldweaksum = lSegment.getWeak();

    // Add the segment to the segment list vector.
    rReceiver.push(lSegment);
    rReport.segmentCount++;

    lCircularBuff.read((char*)lpTempBuffer, 1);

    // Move to the next byte.
    lnOffset += 1;
  }

  // Push an empty segment to mark the end of the stream.
  lSegment.setID(Segment::EndOfStream);
  rReceiver.push(lSegment);

  // Capture the segmentation end time.
  rReport.end.sample();

  // Delete all heap allocations. If we got to this point, we know that the
  // buffers were successfully allocated.
  delete[] lpTempBuffer;

  return true;
}

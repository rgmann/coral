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
bool Segmenter::processEveryOffset(
  std::istream& istream, 
  SegmentReceiver& segment_receiver, 
  ui32 segment_size_bytes,
  SegmentationReport& report
)
{
  static const size_t BLOCK_READ_SIZE = 512;

  ui8*  temp_buffer_ptr = NULL;

  Segment::ID segment_id = 0;

  Segment segment( segment_size_bytes );

  // Offset, in bytes, from the beginning of the file.
  ui32 segment_offset = 0;

  Adler32Checksum   prev_weak_checksum;
  CircularBuffer    circular_buffer;

  if (istream.bad())
  {
    return false;
  }

  // Allocated the buffer to the segment length.
  if ( !circular_buffer.allocate( BLOCK_READ_SIZE ) )
  {
    return false;
  }

  temp_buffer_ptr = new ui8[ segment_size_bytes ];
  if ( temp_buffer_ptr == NULL )
  {
    return false;
  }

  // Capture the start time.
  report.segmentCount = 0;
  report.begin.sample();

  // Record the stride size and segment size;
  report.strideSizeBytes  = 1;
  report.segmentSizeBytes = segment_size_bytes;

  while ((istream.eof() == false) || (circular_buffer.isEmpty() == false))
  {
    if ( (istream.eof() == false) && ( circular_buffer.size() < segment_size_bytes ) )
    {
      circular_buffer.write( istream, BLOCK_READ_SIZE );
    }

    if ( circular_buffer.isEmpty() == false )
    {
      // Create the new segment.  The segment will compute its weak checksum.
      // The Segment always defers its MD5 computation.
      segment.setID( segment_id++ );
      segment.setOffset( segment_offset );
      segment.setData(
        circular_buffer,
        segment_size_bytes,
        ( segment_offset > 0 ) ? &prev_weak_checksum : NULL
      );


      // Get the weak checksum for the next iteration just in case
      // checksum rolling is enabled.
      prev_weak_checksum = segment.getWeak();

      // Add the segment to the segment list vector.
      segment_receiver.push( segment );
      rReport.segmentCount++;

      // Remove one character from the front of the buffer.
      if ( circular_buffer.read( (char*)temp_buffer_ptr, 1 ) != 1 )
      {
        log::error("Segmenter: Failed to read from circular buffer\n");
      }

      // Move to the next byte.
      segment_offset += 1;
    }
  }

  // Push an empty segment to mark the end of the stream.
  segment.setID( Segment::EndOfStream );
  segment_receiver.push( segment );

  // Capture the segmentation end time.
  report.end.sample();

  // Delete all heap allocations. If we got to this point, we know that the
  // buffers were successfully allocated.
  delete[] temp_buffer_ptr;

  return true;
}



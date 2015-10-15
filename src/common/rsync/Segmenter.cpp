// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#include <stdio.h>
#include "Log.h"
#include "CircularBuffer.h"
#include "Segment.h"
#include "SegmentReceiver.h"
#include "SegmentHook.h"
#include "JobReport.h"
#include "Segmenter.h"

using namespace coral;
using namespace coral::rsync;

//-----------------------------------------------------------------------------
bool Segmenter::processFullStride(
  std::istream&       istream,
  SegmentReceiver&    segment_receiver,
  ui32                segment_size_bytes,
  SegmentationReport& report
)
{
   Segment::ID segment_id = 0;
   Segment segment( segment_size_bytes );

   // Offset, in bytes, from the beginning of the file.
   ui32 segment_offset = 0;

   bool segment_success = true;

   if ( istream.bad() )
   {
      segment_success = false;
      report.status = kRsyncIoError;
   }
   else
   {
      // Capture the start time.
      report.segmentCount = 0;
      report.begin.sample();

      ui32 stride_size_bytes = segment_size_bytes;

      // Record the stride size and segment size;
      report.strideSizeBytes  = stride_size_bytes;
      report.segmentSizeBytes = segment_size_bytes;

      while ( istream.eof() == false )
      {
         // Create the new segment.  The segment will compute its weak checksum.
         // The Segment always defers its MD5 computation.
         segment.setID( segment_id++ );
         segment.setOffset( segment_offset );
         segment.setData( istream, segment_size_bytes, NULL );

         // Add the segment to the segment list vector.
         segment_receiver.push( segment );
         report.segmentCount++;

         // Move to the next byte.
         segment_offset += stride_size_bytes;
      }

      // Push an empty segment to mark the end of the stream.
      segment.setID( Segment::EndOfStream );
      segment_receiver.push( segment );

      // Capture the segmentation end time.
      report.end.sample();
      report.status = kRsyncSuccess;
   }

   return segment_success;
}

//-----------------------------------------------------------------------------
bool Segmenter::processEveryOffset(
  std::istream&       istream, 
  SegmentReceiver&    segment_receiver, 
  ui32                segment_size_bytes,
  SegmentationReport& report
)
{
   static const size_t BLOCK_READ_SIZE = 1024;

   ui8*  temp_buffer_ptr = NULL;

   Segment::ID segment_id = 0;

   Segment segment( segment_size_bytes );

   // Offset, in bytes, from the beginning of the file.
   ui32 segment_offset = 0;

   Adler32Checksum   prev_weak_checksum;
   CircularBuffer    circular_buffer;

   bool segment_success = true;

   if (istream.bad())
   {
      segment_success = false;
      report.status = kRsyncIoError;
   }
   else
   {
      // Allocated the buffer to the segment length.
      segment_success &= circular_buffer.allocate( BLOCK_READ_SIZE );

      temp_buffer_ptr = new ui8[ segment_size_bytes ];
      segment_success &= ( temp_buffer_ptr != NULL );

      if ( segment_success == false )
      {
         report.status = kRsyncMemAllocationFailed;
      }
      else
      {
         // Capture the start time.
         report.segmentCount = 0;
         report.begin.sample();

         // Record the stride size and segment size;
         report.strideSizeBytes  = 1;
         report.segmentSizeBytes = segment_size_bytes;

         while ( ( istream.eof() == false ) ||
                 ( circular_buffer.isEmpty() == false ) )
         {
            if ( ( istream.eof() == false ) &&
                 ( circular_buffer.size() < segment_size_bytes ) )
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
               report.segmentCount++;

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

         report.status = kRsyncSuccess;
      }
   }

   // report.complete = true;

   return segment_success;
}



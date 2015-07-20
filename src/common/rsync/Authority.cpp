#include <iostream>
#include <fstream>
#include <algorithm>
#include "Log.h"
#include "Segmenter.h"
#include "JobDescriptor.h"
#include "InstructionReceiver.h"
#include "Authority.h"

using namespace liber;
using namespace liber::rsync;


//-----------------------------------------------------------------------------
Authority::Authority()
: SegmentHook()
, segment_skip_count_(0)
, max_chunk_size_bytes_(512)
, buffered_segment_count_(0)
, buffer_first_segment_id_( Segment::EndOfStream )
, total_segment_bytes_(0)
, segment_bytes_(0)
, chunk_bytes_(0)
, instruction_receiver_ptr(NULL)
{
  chunk_buffer_.allocate( max_chunk_size_bytes_ );
}

//-----------------------------------------------------------------------------
Authority::~Authority()
{
}

//-----------------------------------------------------------------------------
void Authority::reset()
{
  buffered_segment_count_ = 0;
  segment_skip_count_ = 0;
  buffer_first_segment_id_ = -1;

  total_segment_bytes_ = 0;
  segment_bytes_ = 0;
  chunk_bytes_ = 0;

  chunk_buffer_.clear();

  instruction_receiver_ptr = NULL;

  // Delete all Segments remaining in the Segment hash.
  SegmentDestructor destructor;
  segment_hash_.clear( destructor );
}

//-----------------------------------------------------------------------------
bool Authority::process(
  JobDescriptor& job_descriptor,
  std::istream& istream,
  InstructionReceiver& instruction_receiver,
  SourceReport& report)
{
  authority_report_ptr_ = &report.authority;
  
  authority_report_ptr_->receivedSegmentCount = segment_hash_.size();
  authority_report_ptr_->authBegin.sample();

  instruction_receiver_ptr = &instruction_receiver;

  instruction_receiver_ptr->push( new BeginInstruction( job_descriptor ) );
  
  bool segmentation_success = Segmenter::processEveryOffset(
    istream,
    *this,
    job_descriptor.getSegmentSize(),
    report.segmentation
  );
  
  authority_report_ptr_->authEnd.sample();
  instruction_receiver_ptr->push( new EndInstruction() );

  authority_report_ptr_    = NULL;
  instruction_receiver_ptr = NULL;
  
  reset();

  return segmentation_success;
}

//-----------------------------------------------------------------------------
HashTable<16, Segment*>& Authority::hash()
{
  return segment_hash_;
}

//-----------------------------------------------------------------------------
void Authority::flushChunkBuffer( int chunk_flush_count )
{
  if (!chunk_buffer_.isEmpty())
  {
    // Determine how many bytes to send in the chunk instruction. If the 
    // caller specifies 'kFlushAll', all bytes are sent.  Otherwise only the
    // first 'chunk_flush_count' bytes are sent in the chunk and the remainder
    // are thrown away.
    ui32 chunk_size_bytes = chunk_flush_count;

    if ( chunk_flush_count == kFlushAll )
    {
      chunk_size_bytes = chunk_buffer_.size();
    }

    // Allocate the chunk instruction and move the specified number of bytes
    // from the chunk buffer to the chunk instruction.
    ChunkInstruction* instruction_ptr = new ChunkInstruction( chunk_size_bytes );
    if ( instruction_ptr->data() )
    {
      ui32 chunk_buffer_bytes_read = chunk_buffer_.read(
        (char*)instruction_ptr->data(),
        chunk_size_bytes
      );

      if ( chunk_buffer_bytes_read != chunk_size_bytes )
      {
        log::error("Authority: Failed to read from chunk buffer\n");
      }

      // Reset the chunk buffer.
      chunk_buffer_.clear();

      chunk_bytes_ += chunk_size_bytes;
    }

    // Send the chunk instruction to the instruction receiver.
    instruction_receiver_ptr->push( instruction_ptr );

    // All segment data has been removed from the buffer.
    buffered_segment_count_ = 0;
    buffer_first_segment_id_ = Segment::EndOfStream;

    authority_report_ptr_->chunkCount++;
  }
}

//-----------------------------------------------------------------------------
void Authority::call( Segment& segment )
{
  if ( segment.endOfStream() )
  {
    // Make sure all chunk data has been sent.
    flushChunkBuffer();
  }
  else if ( segment_skip_count_ == 0 )
  {
    Segment* match_segment_ptr = NULL;
    SegmentComparator comparator( &segment );

    bool segment_found = segment_hash_.remove(
      segment.getWeak().checksum(),
      match_segment_ptr,
      comparator
    );

    if ( segment_found )
    {
      // Any data in the buffer when a match occurs should be moved to a chunk
      // instruction and sent before sending the ID of the matched segment.
      flushChunkBuffer(( buffer_first_segment_id_ == Segment::EndOfStream ) ?
                       kFlushAll : (segment.getID() - buffer_first_segment_id_));

      // If the Segment exists in the hash, create a Segment instruction.
      if ( match_segment_ptr )
      {
        instruction_receiver_ptr->push(
          new SegmentInstruction( match_segment_ptr->getID() )
        ); 
      }
      else
      {
        log::error("Authority: NULL match segment retrieved from hash.\n");
      }

      // Since the Authority creates a segment for every byte offset, a match
      // allows the Authority to skip the next N-1 segments for a Segment size
      // of N bytes.
      segment_skip_count_ = segment.size() - 1;

      authority_report_ptr_->matchedSegmentCount++;
      segment_bytes_ += segment.size();
      delete match_segment_ptr;
    }
    else
    {
      if (segment.data())
      {
        bool flush_buffer = (
          // The buffer contains a valid chunk data and...
          ( buffer_first_segment_id_ != Segment::EndOfStream ) &&

          // the size of the chunk is at least the minimum configured chunk size.
          ( ( segment.getID() - buffer_first_segment_id_ ) >= max_chunk_size_bytes_ )
        );

        if ( flush_buffer )
        {
          flushChunkBuffer();
        }

        if (buffered_segment_count_ == 0)
        {
          if ( chunk_buffer_.isEmpty() )
          {
            buffer_first_segment_id_ = segment.getID();
          }

          chunk_buffer_.write((const char*)segment.data(), segment.size());
          buffered_segment_count_ = segment.size() - 1;
        }
        else
        {
          buffered_segment_count_--;
        }
      }
      else
      {
        log::debug("NULL data at segment #%d\n", segment.getID());
      }
    }
  }
  else
  {
    segment_skip_count_--;
  }

  total_segment_bytes_ += segment.size();
}




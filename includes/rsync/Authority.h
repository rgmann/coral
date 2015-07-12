#ifndef RSYNC_AUTHORITY_H
#define RSYNC_AUTHORITY_H

#include <fstream>
#include <sstream>
#include "HashTable.h"
#include "JobReport.h"
#include "SegmentHook.h"
// #include "Segmenter.h"
#include "CircularBuffer.h"

namespace liber {
namespace rsync {

class Instruction;
class InstructionReceiver;
class JobDescriptor;

class Authority : public SegmentHook {
public:


  Authority();
  ~Authority();

  /**
   * Takes pre-populated Segment Hash and a local file stream.  The file stream
   * is segmented using the same block size as that used to generate the Segments
   * in the Segment Hash.  Generates a set for instructions for updating the hashed
   * file to match the authoritative file.
   */
  bool process(JobDescriptor& rDescriptor, std::istream& rInStream,
               InstructionReceiver& rReceiver, SourceReport& rReport);

  liber::HashTable<16, Segment*>& hash();

private:

  void reset();

  void call(Segment& rSegment);

  enum { FlushAll = -1 };
  void flushChunkBuffer(int nFlushCount = FlushAll);

private:

  liber::HashTable<16, Segment*>  segment_hash_;

  // Segmenter mSegmenter;

  ui32 segment_skip_count_;
  CircularBuffer chunk_buffer_;

  ui32 max_chunk_size_bytes_;
  ui32 buffered_segment_count_;
  Segment::ID buffer_first_segment_id_;

  ui32 total_segment_bytes_;
  ui32 segment_bytes_;
  ui32 chunk_bytes_;

  AuthorityReport* authority_report_ptr_;

  InstructionReceiver*  instruction_receiver_ptr;

};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_AUTHORITY_H

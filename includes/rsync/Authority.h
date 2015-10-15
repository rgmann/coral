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



#ifndef RSYNC_AUTHORITY_H
#define RSYNC_AUTHORITY_H

#include <fstream>
#include <sstream>
#include "HashTable.h"
#include "JobReport.h"
#include "SegmentHook.h"
#include "CircularBuffer.h"

namespace coral {
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
  bool process(
    JobDescriptor&        job_descriptor,
    std::istream&         istream,
    InstructionReceiver&  instruction_receiver,
    SourceReport&         report
  );

  coral::HashTable<16, Segment*>& hash();

private:

  void reset();

  void addInstruction( Instruction& instruction );
  void addBeginInstruction( JobDescriptor& job_descriptor );
  void addEndInstruction();

  void call( Segment& segment );

  enum { kFlushAll = -1 };
  void flushChunkBuffer( int nFlushCount = kFlushAll );

private:

  //
  // Hash of segments received from the Segmenter
  //
  coral::HashTable<16, Segment*>  segment_hash_;

  //
  // Number of skipped Segments
  //
  ui32 segment_skip_count_;

  //
  // Buffer of chunk data
  //
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
} // End namespace coral

#endif // RSYNC_AUTHORITY_H

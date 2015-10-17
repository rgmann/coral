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



#ifndef RSYNC_INSTRUCTION_H
#define RSYNC_INSTRUCTION_H

#include <fstream>
#include <string.h>
#include <boost/shared_ptr.hpp>
#include "Log.h"
#include "JobDescriptor.h"
#include "Segment.h"
#include "SegmentAccessor.h"
#include "Serializable.h"
#include "RsyncError.h"

namespace coral {
namespace rsync {


class AssemblerState {
public:

   AssemblerState( SegmentAccessor& accessor );

   std::ofstream& stream();

   SegmentAccessor& segmentAccessor();

   JobDescriptor*   descriptor();
   void setDescriptor( JobDescriptor& job_descriptor );

   //
   // Returns true if execution of an instruction failed or if the instruction
   // stream has been cancelled by the Authority.
   //
   bool failed() const;

   //
   // Returns true if the instruction stream has been cancelled by the Authority.
   //
   bool cancelled() const;

   //
   // Returns true if the instruction stream completed successfully.
   //
   bool done() const;

   //
   // Reset reset assembler state in preparation for the next job.
   //
   void reset();

   //
   // Assembly completion status for active job.
   //
   RsyncError  status_;

   //
   // Flag indicating that active assembly job has been completed.
   //
   bool done_;

private:

   std::ofstream     stream_;

   SegmentAccessor&  segment_accessor_;

   JobDescriptor*    job_descriptor_ptr_;
};


class RawInstruction {
public:


   RawInstruction( const void* data_ptr, ui32 length );
   RawInstruction( ui32 type, ui32 length );
   ~RawInstruction();

   //
   //
   //
   enum { kInvalidInstructionType = -1 };
   i32 type() const;


   void* const data() const ;
   ui32 length() const;

   ui32 payload_length() const;
   void* const payload_ptr() const;

   void allocate( ui32 length );
   bool valid() const;
   void dump();


private:

   // Raw instruction data pointer
   ui8* data_ptr_;

   // Size of raw packet
   ui32 length_;

};


typedef  boost::shared_ptr<RawInstruction> RawInstructionPtr;
class Instruction {
public:

   Instruction( i32 type, RawInstructionPtr instruction_ptr );
   virtual ~Instruction();

   //
   //
   //
   bool valid() const;

   //
   // 
   //
   i32  type() const;

   //
   // Access raw instruction backing this Instruction instance.
   //
   RawInstructionPtr instruction();


protected:

   // Instruction type ID
   i32 type_;

   // Shared pointer to RawInstruction backing this Instruction instance
   RawInstructionPtr instruction_ptr_;

};


class BeginInstruction : public Instruction {
public:

   static const ui32 Type = 0;

   //
   //
   //
   BeginInstruction( RawInstructionPtr instruction_ptr = RawInstructionPtr() );

   //
   //
   //
   BeginInstruction(coral::rsync::JobDescriptor& descriptor );

   //
   //
   //
   bool descriptor( coral::rsync::JobDescriptor& descriptor );

};


class SegmentInstruction : public Instruction {
public:

   static const ui32 Type = 1;

   //
   //
   //
   SegmentInstruction( RawInstructionPtr instruction_ptr = RawInstructionPtr() );

   //
   //
   //
   SegmentInstruction(coral::rsync::Segment::ID id);

   //
   //
   //
   coral::rsync::Segment::ID segmentId() const;

};


class ChunkInstruction : public Instruction {
public:

   static const ui32 Type = 2;

   //
   //
   //
   ChunkInstruction( RawInstructionPtr instruction_ptr = RawInstructionPtr() );

   //
   //
   //
   ChunkInstruction( ui32 length );

   //
   //
   //
   ui8* const data();

   //
   //
   //
   ui32 size() const;

};


class EndInstruction : public Instruction {
public:

   static const ui32 Type = 3;

   struct __attribute__((packed)) data {
      bool canceled;
      ui32 cancel_error;
   };

   //
   //
   //
   EndInstruction( RawInstructionPtr instruction_ptr );

   //
   //
   //
   EndInstruction();

   //
   //
   //
   void cancel( RsyncError error );

   //
   //
   //
   bool canceled() const;

   //
   //
   //
   RsyncError error() const;

   //
   //
   //
   data* const data_ptr() const;

};

} // End namespace rsync
} // End namespace coral


#endif // RSYNC_INSTRUCTION_H


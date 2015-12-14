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



#include <sstream>
#include "Log.h"
#include "Instruction.h"

#undef DEBUG_RSYNC_INSTRUCTIONS

using namespace coral::rsync;
using namespace coral::netapp;

const i32 BeginInstruction::Type;
const i32 SegmentInstruction::Type;
const i32 ChunkInstruction::Type;
const i32 EndInstruction::Type;

//-----------------------------------------------------------------------------
AssemblerState::AssemblerState( SegmentAccessor& accessor )
: segment_accessor_( accessor )
{
   reset();
}

//-----------------------------------------------------------------------------
std::ofstream& AssemblerState::stream()
{
   return stream_;
}

//-----------------------------------------------------------------------------
SegmentAccessor& AssemblerState::segmentAccessor()
{
   return segment_accessor_;
}

//-----------------------------------------------------------------------------
JobDescriptor* AssemblerState::descriptor()
{
   return job_descriptor_ptr_;
}

//-----------------------------------------------------------------------------
void AssemblerState::setDescriptor( JobDescriptor& job_descriptor )
{
   job_descriptor_ptr_ = &job_descriptor;
}

//-----------------------------------------------------------------------------
bool AssemblerState::failed() const
{
   return ( status_ != kRsyncSuccess );
}

//-----------------------------------------------------------------------------
bool AssemblerState::cancelled() const
{
   return ( status_ == kRsyncJobCanceled );
}

//-----------------------------------------------------------------------------
bool AssemblerState::done() const
{
   return done_;
}

//-----------------------------------------------------------------------------
void AssemblerState::reset()
{
   status_ = kRsyncSuccess;
   done_   = false;
   job_descriptor_ptr_ = NULL;
}


//-----------------------------------------------------------------------------
// Class: Instruction
//-----------------------------------------------------------------------------
Instruction::Instruction( i32 type, RawInstructionPtr instruction_ptr )
:  type_             ( type )
,  instruction_ptr_  ( instruction_ptr )
{
}

//-----------------------------------------------------------------------------
Instruction::~Instruction()
{
}

//-----------------------------------------------------------------------------
bool Instruction::valid() const
{
   return (
      instruction_ptr_ &&
      instruction_ptr_->valid() &&
      ( instruction_ptr_->type() == type_ )
   );
}

//-----------------------------------------------------------------------------
i32 Instruction::type() const
{
   if ( valid() )
   {
      return instruction_ptr_->type();
   }
   else
   {
      return RawInstruction::kInvalidInstructionType;
   }
}

//-----------------------------------------------------------------------------
RawInstructionPtr Instruction::instruction()
{
   return instruction_ptr_;
}


//-----------------------------------------------------------------------------
// Class: RawInstruction
//-----------------------------------------------------------------------------
RawInstruction::RawInstruction( const void* data_ptr, ui32 length )
:  data_ptr_   ( NULL )
,  length_     ( 0 )
{
   allocate( length );
   memcpy( data_ptr_, data_ptr, length_ );
}
//-----------------------------------------------------------------------------
RawInstruction::RawInstruction( i32 type, ui32 length )
:  data_ptr_ ( NULL )
,  length_ ( 0 )
{
   allocate( length + sizeof( i32 ) );
   i32* type_ptr  = (i32*)data_ptr_;
   *type_ptr      = type;
}
//-----------------------------------------------------------------------------
RawInstruction::~RawInstruction()
{
   if ( data_ptr_ != NULL )
   {
      delete[] data_ptr_;
   }
}

//-----------------------------------------------------------------------------
i32 RawInstruction::type() const
{
   i32 type = kInvalidInstructionType;

   if ( length_ >= sizeof( i32 ) )
   {
      type = *((i32*)data_ptr_);
   }

   return type;
}

//-----------------------------------------------------------------------------
ui32 RawInstruction::length() const
{
   return length_;
}

//-----------------------------------------------------------------------------
ui32 RawInstruction::payload_length() const
{
   ui32 payload_length = 0;

   if ( length_ > sizeof( i32 ) )
   {
      payload_length = length_ - sizeof(i32);
   }

   return payload_length;
}

//-----------------------------------------------------------------------------
void* const RawInstruction::data() const
{
   return data_ptr_;
}

//-----------------------------------------------------------------------------
void* const RawInstruction::payload_ptr() const
{
   if ( data_ptr_ && ( length_ > sizeof( i32 ) ) )
   {
      return ( (ui8*)data_ptr_ + sizeof( i32 ) );
   }

   return NULL;
}

//-----------------------------------------------------------------------------
void RawInstruction::allocate( ui32 length )
{
   if ( data_ptr_ != NULL )
   {
      delete[] data_ptr_;
      data_ptr_ = NULL;
   }

   data_ptr_ = new ui8[ length ];
   length_   = length;

   memset( data_ptr_, 0, length_ );
}

//-----------------------------------------------------------------------------
bool RawInstruction::valid() const {
   return (
      payload_ptr() &&
      ( type() != RawInstruction::kInvalidInstructionType )
   );
}

//-----------------------------------------------------------------------------
void RawInstruction::dump()
{
   if ( valid() )
   {
      coral::log::mem_dump("", (const char*)data(), length() );
   }
}


//-----------------------------------------------------------------------------
// Class: BeginInstruction
//-----------------------------------------------------------------------------
BeginInstruction::BeginInstruction( RawInstructionPtr instruction_ptr )
:  Instruction( BeginInstruction::Type, instruction_ptr )
{
}

//-----------------------------------------------------------------------------
BeginInstruction::BeginInstruction(coral::rsync::JobDescriptor& descriptor )
:  Instruction( BeginInstruction::Type, RawInstructionPtr() )
{
   std::string packed = descriptor.serialize();

   instruction_ptr_ =
      RawInstructionPtr( new RawInstruction( Type, packed.size() ) );

   memcpy( instruction_ptr_->payload_ptr(), packed.data(), packed.size() );
}

//-----------------------------------------------------------------------------
bool BeginInstruction::descriptor( coral::rsync::JobDescriptor& descriptor )
{
   bool success = false;

   if ( instruction_ptr_.get() != NULL )
   {
      success = descriptor.deserialize(
         (const char*)instruction_ptr_->payload_ptr(),
         instruction_ptr_->payload_length() );
   }

   return success;
}

//-----------------------------------------------------------------------------
// Class: SegmentInstruction
//-----------------------------------------------------------------------------
SegmentInstruction::SegmentInstruction( RawInstructionPtr instruction_ptr )
:  Instruction( SegmentInstruction::Type, instruction_ptr )
{
}

//-----------------------------------------------------------------------------
SegmentInstruction::SegmentInstruction( coral::rsync::Segment::ID id )
: Instruction( SegmentInstruction::Type, RawInstructionPtr() )
{
   instruction_ptr_ =
      RawInstructionPtr( new RawInstruction( Type, sizeof( id ) ) );
   Segment::ID* data_ptr = (Segment::ID*)instruction_ptr_->payload_ptr();
   *data_ptr = id;
}

// //-----------------------------------------------------------------------------
coral::rsync::Segment::ID SegmentInstruction::segmentId() const
{
   Segment::ID segment_id = -1;

   if ( instruction_ptr_ )
   {
      segment_id = *( (Segment::ID*)instruction_ptr_->payload_ptr() );
   }

   return segment_id;
}


//-----------------------------------------------------------------------------
// Class: SegmentInstruction
//-----------------------------------------------------------------------------
ChunkInstruction::ChunkInstruction( RawInstructionPtr instruction_ptr )
:  Instruction( ChunkInstruction::Type, instruction_ptr )
{
}

//-----------------------------------------------------------------------------
ChunkInstruction::ChunkInstruction( ui32 length )
: Instruction( ChunkInstruction::Type, RawInstructionPtr() )
{
   instruction_ptr_ = RawInstructionPtr( new RawInstruction( Type, length ) );
}

//-----------------------------------------------------------------------------
ui8* const ChunkInstruction::data()
{
   if ( instruction_ptr_ )
   {
      return (ui8* const)instruction_ptr_->payload_ptr();
   }
   else
   {
      return NULL;
   }
}

//-----------------------------------------------------------------------------
ui32 ChunkInstruction::size() const
{
   if ( instruction_ptr_ )
   {
      return instruction_ptr_->payload_length();
   }
   else
   {
      return 0;
   }
}

//-----------------------------------------------------------------------------
// Class: EndInstruction
//-----------------------------------------------------------------------------
EndInstruction::EndInstruction( RawInstructionPtr instruction_ptr )
:  Instruction( EndInstruction::Type, instruction_ptr )
{
}

//-----------------------------------------------------------------------------
EndInstruction::EndInstruction()
:  Instruction( EndInstruction::Type, RawInstructionPtr() )
{
   instruction_ptr_ =
      RawInstructionPtr( new RawInstruction( Type, sizeof( data ) ) );
}

//-----------------------------------------------------------------------------
void EndInstruction::cancel(RsyncError error)
{
   if ( instruction_ptr_ )
   {
      data_ptr()->canceled = true;
      data_ptr()->cancel_error = error;
   }
}

//-----------------------------------------------------------------------------
bool EndInstruction::canceled() const
{
   bool canceled = true;

   if ( data_ptr() )
   {
      canceled = data_ptr()->canceled;
   }

   return canceled;
}

//-----------------------------------------------------------------------------
RsyncError EndInstruction::error() const
{
   RsyncError error = kRsyncIoError;

   if ( data_ptr() )
   {
      error = (RsyncError)data_ptr()->cancel_error;
   }

   return error;
}

//-----------------------------------------------------------------------------
EndInstruction::data* const EndInstruction::data_ptr() const
{
   if ( instruction_ptr_ )
   {
      return (data*)instruction_ptr_->payload_ptr();
   }
   else
   {
      return NULL;
   }
}

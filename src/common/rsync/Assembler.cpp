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


#include "Log.h"
#include "InstructionQueue.h"
#include "SegmentAccessor.h"
#include "JobReport.h"
#include "Assembler.h"

using namespace coral;
using namespace coral::rsync;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
Assembler::Assembler( SegmentAccessor& segment_accessor )
: assembler_state_( segment_accessor )
{
}

//-----------------------------------------------------------------------------
Assembler::~Assembler()
{
}

//-----------------------------------------------------------------------------
bool Assembler::process(
   JobDescriptor&    job_descriptor,
   InstructionQueue& instruction_queue,
   AssemblyReport&   report
)
{
   assembler_state_.reset();
   assembler_state_.setDescriptor( job_descriptor );

   while ( ( assembler_state_.done() == false ) &&
           ( assembler_state_.failed() == false ) )
   {
      RawInstructionPtr instruction_ptr = instruction_queue.pop(
         job_descriptor.completionTimeoutMs()
      );

      if ( instruction_ptr )
      {
         if ( instruction_ptr->valid() )
         {
            switch ( instruction_ptr->type() )
            {
               case BeginInstruction::Type:
                  report.begin.sample();
                  assembler_state_.status_ = processBeginInstruction(
                     instruction_ptr );
                  break;

               case SegmentInstruction::Type:
                  report.segmentCount++;
                  assembler_state_.status_ = processSegmentInstruction(
                     instruction_ptr );
                  break;

               case ChunkInstruction::Type:
                  report.chunkCount++;
                  assembler_state_.status_ = processChunkInstruction(
                     instruction_ptr );
                  break;

               case EndInstruction::Type:
                  report.end.sample();
                  assembler_state_.status_ = processEndInstruction(
                     instruction_ptr );
                  break;

               default:
                  log::error(
                     "Assembler - Invalid instruction type %d\n",
                     instruction_ptr->type());

                  assembler_state_.status_ = kRsyncInvalidInstruction;
                  break;
            }
         }
         else
         {
            assembler_state_.status_ = kRsyncIoError;
         }

         if ( assembler_state_.failed() )
         {
            log::error(
               "Assembler::process - Assembly failed with status %s\n",
               errorToString( assembler_state_.status_ ).c_str()
            );
         }
      }
      else
      {
         log::error("Assembler - NULL instruction.\n");
      }
   }

   report.status = assembler_state_.status_;

   return ( assembler_state_.failed() == false );
}

//-----------------------------------------------------------------------------
std::ofstream& Assembler::outputStream()
{
  return assembler_state_.stream();
}

//-----------------------------------------------------------------------------
RsyncError Assembler::processBeginInstruction( RawInstructionPtr instruction_ptr )
{
   RsyncError status = kRsyncSuccess;

   BeginInstruction instruction( instruction_ptr );

   if ( instruction.valid() )
   {
      JobDescriptor descriptor( instruction_ptr->payload_ptr(),
                                instruction_ptr->payload_length() );

      if ( descriptor.isValid() && assembler_state_.descriptor() )
      {
         if ( descriptor.getDestination().path() != 
              assembler_state_.descriptor()->getDestination().path() )
         {
            status = kRsyncInvalidJob;
         }
      }
      else
      {
         status = kRsyncInvalidJob;
      }
   }
   else
   {
      status = kRsyncCommError;
   }

   return status;
}

//-----------------------------------------------------------------------------
RsyncError Assembler::processChunkInstruction( RawInstructionPtr instruction_ptr )
{
   RsyncError status = kRsyncSuccess;

   ChunkInstruction instruction( instruction_ptr );

   if ( instruction.valid() )
   {
      if ( instruction.data() && assembler_state_.descriptor() )
      {
         // Validate the chunk size
         if ( ( instruction.size() > 0 ) &&
              ( instruction.size() <=
               assembler_state_.descriptor()->getMaximumChunkSize() ) )
         {
            assembler_state_.stream().write(
               (char*)instruction.data(),
               instruction.size()
            );

            #ifdef DEBUG_RSYNC_INSTRUCTIONS
            log::debug("Executing ChunkInstruction:\n");
            for (int ind = 0; ind < instruction.size(); ind++)
            {
              printf("0x%02X ", instruction.data()[ind]);
              if ((ind > 0) && (ind % 16 == 0)) printf("\n");
            }
            printf("\n");
            #endif

            if ( assembler_state_.stream().fail() )
            {
               status = kRsyncIoError;
            }
         }
         else
         {
            status = kRsyncAssemblerInvalidChunkSize;
         }
      }
      else
      {
         status = kRsyncIoError;
         log::error("ChunkInstruction: NULL data or failed ostream.\n");
      }
   }
   else
   {
      status = kRsyncCommError;
   }

   return status;
}

//-----------------------------------------------------------------------------
RsyncError Assembler::processSegmentInstruction( RawInstructionPtr instruction_ptr )
{
   RsyncError status = kRsyncSuccess;

   SegmentInstruction instruction( instruction_ptr );

   if ( instruction.valid() )
   {
      Segment* segment_ptr = assembler_state_.segmentAccessor().getSegment(
         instruction.segmentId()
      );

      if ( segment_ptr != NULL )
      {
         assembler_state_.stream().write(
            (char*)segment_ptr->data(),
            segment_ptr->size()
         );

         if ( assembler_state_.stream().fail() )
         {
            status = kRsyncIoError;
            log::error(
               "SegmentInstruction: Failed to write segment to ostream.\n" );
         }

         #ifdef DEBUG_RSYNC_INSTRUCTIONS
         log::debug("Executing SegmentInstruction:\n%s\n",
                  segment_ptr->debugDump().c_str());
         #endif
      }
      else
      {
         status = kRsyncSegmentAccessError;
         log::error("SegmentInstruction: Failed to access segment.\n");
      }
   }
   else
   {
      status = kRsyncCommError;
   }

   return status;
}

//-----------------------------------------------------------------------------
RsyncError Assembler::processEndInstruction( RawInstructionPtr instruction_ptr )
{
   RsyncError status = kRsyncSuccess;

   EndInstruction instruction( instruction_ptr );

   if ( instruction.valid() )
   {
      if ( instruction.canceled() )
      {
         status = kRsyncJobCanceled;
      }
      else
      {
         assembler_state_.done_ = true;
      }

      assembler_state_.stream().close();
   }
   else
   {
      status = kRsyncCommError;
   }

   return status;
}


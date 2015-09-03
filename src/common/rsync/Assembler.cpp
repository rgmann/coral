#include "Log.h"
#include "InstructionQueue.h"
#include "SegmentAccessor.h"
#include "JobReport.h"
#include "Assembler.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

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
      InstructionRaw* instruction_ptr = instruction_queue.pop(
         job_descriptor.completionTimeoutMs()
      );

      if ( instruction_ptr )
      {
         if ( instruction_ptr->valid() )
         {
            // log::status("Assembler::process: %d\n", instruction_ptr->length());
            // instruction_ptr->dump();
            switch ( instruction_ptr->type() )
            {
               case BeginInstruction::Type:
                  report.begin.sample();
                  processBeginInstruction( instruction_ptr );
                  break;

               case SegmentInstruction::Type:
                  report.segmentCount++;
                  processSegmentInstruction( instruction_ptr );
                  break;

               case ChunkInstruction::Type:
                  report.chunkCount++;
                  processChunkInstruction( instruction_ptr );
                  break;

               case EndInstruction::Type:
                  report.end.sample();
                  processEndInstruction( instruction_ptr );
                  break;

               default:
                  log::error("Assembler - Invalid instruction type %d\n",
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

         delete instruction_ptr;
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
void Assembler::processBeginInstruction( InstructionRaw* instruction_ptr )
{
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
            assembler_state_.status_ = kRsyncInvalidJob;
         }
      }
      else
      {
         assembler_state_.status_ = kRsyncInvalidJob;
      }
   }
   else
   {
      assembler_state_.status_ = kRsyncCommError;
   }

   instruction.release();
}

//-----------------------------------------------------------------------------
void Assembler::processChunkInstruction( InstructionRaw* instruction_ptr )
{
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
               assembler_state_.status_ = kRsyncIoError;
            }
         }
         else
         {
            assembler_state_.status_ = kRsyncAssemblerInvalidChunkSize;
         }
      }
      else
      {
         assembler_state_.status_ = kRsyncIoError;
         log::error("ChunkInstruction: NULL data or failed ostream.\n");
      }
   }
   else
   {
      assembler_state_.status_ = kRsyncCommError;
   }

   instruction.release();
}

//-----------------------------------------------------------------------------
void Assembler::processSegmentInstruction( InstructionRaw* instruction_ptr )
{
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
            assembler_state_.status_ = kRsyncIoError;
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
         assembler_state_.status_ = kRsyncSegmentAccessError;
         log::error("SegmentInstruction: Failed to access segment.\n");
      }
   }
   else
   {
      assembler_state_.status_ = kRsyncCommError;
   }

   instruction.release();
}

//-----------------------------------------------------------------------------
void Assembler::processEndInstruction( InstructionRaw* instruction_ptr )
{
   EndInstruction instruction( instruction_ptr );

   if ( instruction.valid() )
   {
      if ( instruction.canceled() )
      {
         assembler_state_.status_ = kRsyncJobCanceled;
      }
      else
      {
         assembler_state_.done_ = true;
      }

      assembler_state_.stream().close();
   }
   else
   {
      assembler_state_.status_ = kRsyncCommError;
   }

   instruction.release();
}


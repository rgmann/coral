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
      InstructionContainer* container_ptr = instruction_queue.pop(
         job_descriptor.completionTimeoutMs()
      );

      if ( container_ptr )
      {
         if ( container_ptr->deserialize( container_ptr->stream() ) )
         {
            switch ( container_ptr->type() )
            {
               case BeginInstruction::Type:
                  report.begin.sample();
                  processBeginInstruction( container_ptr->stream() );
                  break;

               case SegmentInstruction::Type:
                  report.segmentCount++;
                  processSegmentInstruction( container_ptr->stream() );
                  break;

               case ChunkInstruction::Type:
                  report.chunkCount++;
                  processChunkInstruction( container_ptr->stream() );
                  break;

               case EndInstruction::Type:
                  report.end.sample();
                  processEndInstruction( container_ptr->stream() );
                  break;

               default:
                  log::error("Assembler - Invalid instruction type %d\n",
                           container_ptr->type());
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

         delete container_ptr;
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
void Assembler::processBeginInstruction( SerialStream& stream )
{
   BeginInstruction instruction;

   if ( instruction.deserialize( stream ) )
   {
      if ( instruction.descriptor().getDestination().path() != 
           assembler_state_.descriptor().getDestination().path() )
      {
         assembler_state_.status_ = kRsyncInvalidJob;
      }
   }
   else
   {
      assembler_state_.status_ = kRsyncCommError;
   }
}

//-----------------------------------------------------------------------------
void Assembler::processChunkInstruction( SerialStream& stream )
{
   ChunkInstruction instruction;

   if ( instruction.deserialize( stream ) )
   {
      if ( instruction.chunk_data_ptr_ )
      {
         // Validate the chunk size
         if ( ( instruction.chunk_size_bytes_ > 0 ) &&
              ( instruction.chunk_size_bytes_ <=
               assembler_state_.descriptor().getMaximumChunkSize() ) )
         {
            assembler_state_.stream().write(
               (char*)instruction.chunk_data_ptr_,
               instruction.chunk_size_bytes_
            );

            #ifdef DEBUG_RSYNC_INSTRUCTIONS
            log::debug("Executing ChunkInstruction:\n");
            for (int ind = 0; ind < instruction.chunk_size_bytes_; ind++)
            {
              printf("0x%02X ", instruction.chunk_data_ptr_[ind]);
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
}

//-----------------------------------------------------------------------------
void Assembler::processSegmentInstruction( SerialStream& stream )
{
   SegmentInstruction instruction;
   if ( instruction.deserialize( stream ) )
   {
      Segment* segment_ptr = assembler_state_.segmentAccessor().getSegment(
         instruction.segment_id_
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
}

//-----------------------------------------------------------------------------
void Assembler::processEndInstruction( SerialStream& stream )
{
   EndInstruction instruction;

   if ( instruction.deserialize( stream ) )
   {
      if ( instruction.canceled_ )
      {
         assembler_state_.status_ = kRsyncJobCanceled;
      }
      else
      {
         assembler_state_.done_ = true;
      }

      assembler_state_.stream().close();
   }
}


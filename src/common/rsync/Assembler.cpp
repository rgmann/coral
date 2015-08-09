#include "Log.h"
#include "InstructionQueue.h"
#include "SegmentAccessor.h"
#include "JobReport.h"
#include "Assembler.h"

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
Assembler::Assembler( SegmentAccessor& segment_accessor )
: assembler_state_        ( segment_accessor )
{
}

//-----------------------------------------------------------------------------
Assembler::~Assembler()
{
}

//-----------------------------------------------------------------------------
bool Assembler::process(
  JobDescriptor& job_descriptor,
  InstructionQueue& instruction_queue,
  AssemblyReport&   report
)
{
  assembler_state_.reset();
  assembler_state_.jobDescriptor() = job_descriptor;

  while ( ( assembler_state_.done() == false ) &&
          ( assembler_state_.failed() == false ) )
  {
    Instruction* instruction_ptr = instruction_queue.pop(
      job_descriptor.completionTimeoutMs()
    );

    if ( instruction_ptr )
    {
      switch ( instruction_ptr->type() )
      {
        case BeginInstruction::Type:
          report.begin.sample();
          break;

        case SegmentInstruction::Type:
          report.segmentCount++;
          break;

        case ChunkInstruction::Type:
          report.chunkCount++;
          break;

        case EndInstruction::Type:
          report.end.sample();
          break;

        default:
          log::error("Assembler - Invalid instruction type %d\n",
                     instruction_ptr->type());
          break;
      }

      instruction_ptr->execute(
        assembler_state_
      );

      if ( assembler_state_.failed() )
      {
        log::error(
          "Assembler::process - "
          "Instruction (type=%d) failed with status %d\n",
          instruction_ptr->type(),
          assembler_state_.status_
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


#include "log.h"
#include "InstructionQueue.h"
#include "SegmentAccessor.h"
#include "JobReport.h"
#include "Assembler.h"

#define  DEFAULT_INST_TIMEOUT_MS     (100000)

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
Assembler::Assembler( SegmentAccessor& segment_accessor )
: segment_accessor_       ( segment_accessor )
, instruction_timeout_ms_ ( DEFAULT_INST_TIMEOUT_MS )
{
}

//-----------------------------------------------------------------------------
Assembler::~Assembler()
{
}

//-----------------------------------------------------------------------------
bool Assembler::process(
  InstructionQueue& instruction_queue,
  AssemblyReport&   report
)
{
  assembly_status_.reset();

  while ( ( assembly_status_.done() == false ) &&
          ( assembly_status_.failed() == false ) )
  {
    Instruction* instruction_ptr = instruction_queue.pop(
      instruction_timeout_ms_
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
        assembly_status_,
        segment_accessor_,
        output_stream_
      );

      if ( assembly_status_.failed() )
      {
        log::error(
          "Assembler::process - "
          "Instruction (type=%d) failed with status %d\n",
          instruction_ptr->type(),
          assembly_status_.error
        );
      }

      delete instruction_ptr;
    }
    else
    {
      log::error("Assembler - NULL instruction.\n");
    }
  }

  return ( assembly_status_.failed() == false );
}

//-----------------------------------------------------------------------------
const ExecutionStatus& Assembler::status() const
{
  return assembly_status_;
}

//-----------------------------------------------------------------------------
std::ofstream& Assembler::outputStream()
{
  return output_stream_;
}


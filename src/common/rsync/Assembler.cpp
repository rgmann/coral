#include "log.h"
#include "InstructionQueue.h"
#include "JobReport.h"
#include "Assembler.h"

#define  DEFAULT_INST_TIMEOUT_MS     (100000)

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
Assembler::
Assembler(SegmentAccessor& rAccessor)
: mrAccessor(rAccessor)
, mnInstructionTimeoutMs(DEFAULT_INST_TIMEOUT_MS)
{
}

//-----------------------------------------------------------------------------
Assembler::~Assembler()
{
}

//-----------------------------------------------------------------------------
bool Assembler::process(InstructionQueue& rQueue, AssemblyReport& rReport)
{
  mStatus.reset();

  while ((mStatus.done() == false) && (mStatus.failed() == false))
  {
    Instruction* lpInstruction = rQueue.pop(mnInstructionTimeoutMs);

    if (lpInstruction)
    {
      switch (lpInstruction->type())
      {
        case BeginInstruction::Type:
          rReport.begin.sample();
          break;

        case SegmentInstruction::Type:
          rReport.segmentCount++;
          break;

        case ChunkInstruction::Type:
          rReport.chunkCount++;
          break;

        case EndInstruction::Type:
          rReport.end.sample();
          break;

        default:
          log::error("Assembler - Invalid instruction type %d\n",
                     lpInstruction->type());
          break;
      }

      lpInstruction->execute(mStatus, mrAccessor, mOStream);
      delete lpInstruction;
    }
    else
    {
      log::error("Assembler - NULL instruction.\n");
    }
  }

  return (mStatus.failed() == false);
}

//-----------------------------------------------------------------------------
const ExecutionStatus& Assembler::status() const
{
  return mStatus;
}

//-----------------------------------------------------------------------------
std::ofstream& Assembler::outputStream()
{
  return mOStream;
}


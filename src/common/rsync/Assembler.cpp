#include "InstructionQueue.h"
#include "JobReport.h"
#include "Assembler.h"

#define  DEFAULT_INST_TIMEOUT_MS     (100000)

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

        default: break;
      }

      lpInstruction->execute(mStatus, mrAccessor, mOStream);
      delete lpInstruction;
    }
    else
    {
    }
  }

  return (mStatus.failed() == false);
}

//-----------------------------------------------------------------------------
/*bool Assembler::execute(Instruction* pInstruction)
{
  bool lbCanExecute = true;

  // Pre-process the command.  For example, the Assembler does not execute
  // the BeginInstruction.
  if (lbCanExecute)
  {
    pInstruction->execute(mStatus, mrAccessor, mOStream);
  }

  return (mStatus.failed() == false);
}*/

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


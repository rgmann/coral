#include "Log.h"
#include "InstructionQueue.h"

using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
InstructionQueue::InstructionQueue()
: InstructionReceiver()
{
  mInstructions.initialize();
}

//----------------------------------------------------------------------------
InstructionQueue::~InstructionQueue()
{
}

//----------------------------------------------------------------------------
void InstructionQueue::push(Instruction* const pInstruction)
{
  if (pInstruction)
  {
    mInstructions.push(pInstruction);
  }
  else
  {
    log::error("InstructionQueue::push: Attempting to push NULL instruction.\n");
  }
}

//----------------------------------------------------------------------------
Instruction* InstructionQueue::pop(int nTimeoutMs)
{
  Instruction* lpInstruction = NULL;
  mInstructions.pop(lpInstruction, nTimeoutMs);
  return lpInstruction;
}


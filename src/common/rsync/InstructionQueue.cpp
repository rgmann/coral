#include "Log.h"
#include "InstructionQueue.h"

using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
InstructionQueue::InstructionQueue()
: InstructionReceiver()
{
  instructions_.initialize();
}

//----------------------------------------------------------------------------
InstructionQueue::~InstructionQueue()
{
}

//----------------------------------------------------------------------------
void InstructionQueue::push( Instruction* instruction_ptr )
{
  if ( instruction_ptr )
  {
    if ( instructions_.push( instruction_ptr ) == false )
    {
      log::status("InstructionQueue::push: Failed to push\n");
    }
  }
  else
  {
    log::error("InstructionQueue::push: Attempting to push NULL instruction.\n");
  }
}

//----------------------------------------------------------------------------
Instruction* InstructionQueue::pop( int timeout_ms )
{
  Instruction* instruction_ptr = NULL;
  instructions_.pop( instruction_ptr, timeout_ms );
  return instruction_ptr;
}


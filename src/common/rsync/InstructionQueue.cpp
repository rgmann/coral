#include "Log.h"
#include "InstructionQueue.h"

using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
InstructionQueue::InstructionQueue()
:  InstructionReceiver()
{
}

//----------------------------------------------------------------------------
InstructionQueue::~InstructionQueue()
{
}

//----------------------------------------------------------------------------
void InstructionQueue::push( InstructionRaw* instruction_ptr )
{
   if ( instruction_ptr )
   {
      if ( containers_.push( instruction_ptr ) == false )
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
InstructionRaw* InstructionQueue::pop( int timeout_ms )
{
   InstructionRaw* instruction_ptr = NULL;
   containers_.pop( instruction_ptr, timeout_ms );
   return instruction_ptr;
}


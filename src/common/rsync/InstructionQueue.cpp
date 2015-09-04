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
void InstructionQueue::push( RawInstructionPtr instruction_ptr )
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
RawInstructionPtr InstructionQueue::pop( int timeout_ms )
{
   RawInstructionPtr instruction_ptr;
   containers_.pop( instruction_ptr, timeout_ms );
   return instruction_ptr;
}


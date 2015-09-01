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
void InstructionQueue::push( InstructionContainer* container_ptr )
{
   if ( container_ptr )
   {
      if ( containers_.push( container_ptr ) == false )
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
InstructionContainer* InstructionQueue::pop( int timeout_ms )
{
   InstructionContainer* container_ptr = NULL;
   containers_.pop( container_ptr, timeout_ms );
   return container_ptr;
}


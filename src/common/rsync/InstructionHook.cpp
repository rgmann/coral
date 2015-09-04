#include "InstructionHook.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
InstructionHook::InstructionHook()
:  InstructionReceiver()
{
}

//-----------------------------------------------------------------------------
InstructionHook::~InstructionHook()
{
}

//-----------------------------------------------------------------------------
void InstructionHook::push( RawInstructionPtr instruction_ptr )
{
   if ( instruction_ptr )
   {
      call( instruction_ptr );
   }
}


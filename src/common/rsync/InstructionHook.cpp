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
void InstructionHook::push( InstructionRaw* instruction_ptr )
{
   if ( instruction_ptr )
   {
      call( instruction_ptr );
   }
}


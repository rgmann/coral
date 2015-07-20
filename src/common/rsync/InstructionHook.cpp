#include "InstructionHook.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
InstructionHook::InstructionHook()
: InstructionReceiver()
{
}

//-----------------------------------------------------------------------------
InstructionHook::~InstructionHook()
{
}

//-----------------------------------------------------------------------------
void InstructionHook::push( Instruction* instruction_ptr )
{
  call( instruction_ptr );
}


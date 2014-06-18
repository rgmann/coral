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
void InstructionHook::push(Instruction* pInstruction)
{
  call(pInstruction);
}


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
void InstructionHook::push( InstructionContainer* container_ptr )
{
  call( container_ptr );
}


#include "InstructionList.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
InstructionList::InstructionList()
{
}

//-----------------------------------------------------------------------------
InstructionList::~InstructionList()
{
}

//-----------------------------------------------------------------------------
void InstructionList::push( Instruction* instruction_ptr )
{
  if ( instruction_ptr )
  {
    instructions_.push_back( instruction_ptr );
  }
}

//-----------------------------------------------------------------------------
std::vector<Instruction*>& InstructionList::instructions()
{
  return instructions_;
}


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
void InstructionList::push(Instruction* pInstruction)
{
  if (pInstruction)
  {
    mInstructions.push_back(pInstruction);
  }
}

//-----------------------------------------------------------------------------
std::vector<Instruction*>& InstructionList::instructions()
{
  return mInstructions;
}


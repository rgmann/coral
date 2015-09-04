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
void InstructionList::push( RawInstructionPtr instruction_ptr )
{
   if ( instruction_ptr )
   {
      containers_.push_back( instruction_ptr );
   }
}

//-----------------------------------------------------------------------------
std::vector<RawInstructionPtr>& InstructionList::instructions()
{
   return containers_;
}


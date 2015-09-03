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
void InstructionList::push( InstructionRaw* instruction_ptr )
{
   if ( instruction_ptr )
   {
      containers_.push_back( instruction_ptr );
   }
}

//-----------------------------------------------------------------------------
std::vector<InstructionRaw*>& InstructionList::instructions()
{
   return containers_;
}


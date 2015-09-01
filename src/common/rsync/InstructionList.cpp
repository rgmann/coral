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
void InstructionList::push( InstructionContainer* container_ptr )
{
   if ( container_ptr )
   {
      containers_.push_back( container_ptr );
   }
}

//-----------------------------------------------------------------------------
std::vector<InstructionContainer*>& InstructionList::instructions()
{
   return containers_;
}


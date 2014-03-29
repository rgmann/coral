#include "Assembler.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
Assembler::
Assembler(SegmentAccessor& rAccessor, std::ofstream& ostream)
: mrAccessor(rAccessor)
, mrOStream(ostream)
{
}

//-----------------------------------------------------------------------------
Assembler::~Assembler()
{
}

//-----------------------------------------------------------------------------
bool Assembler::execute(Instruction* pInstruction)
{
  bool lbSuccess = true;
  bool lbCanExecute = true;

  // Pre-process the command.  For example, the Assembler does not execute
  // the BeginInstruction.
/*  switch (pInstruction->type())
  {
    case BeginInstruction::Type:
      lbCanExecute = false;
      break;

    default: break;
  }*/

  if (lbCanExecute)
  {
    lbSuccess = pInstruction->execute(mrAccessor, mrOStream);
  }

  return lbSuccess;
}


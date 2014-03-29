#include <stdio.h>
#include "HeimdallLedStateCommand.h"
#include "ByteOrder.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace eterm;

//-----------------------------------------------------------------------------
HeimdallLedStateCommand::HeimdallLedStateCommand()
: GenericPacket(sizeof(HeimdallLedStateCommand::Data), 0)
{
}

//-----------------------------------------------------------------------------
HeimdallLedStateCommand::Data* const HeimdallLedStateCommand::data()
{
  Data* lpHeader = NULL;

  if (isAllocated())
  {
    lpHeader = reinterpret_cast<Data*>(basePtr());
  }

  return lpHeader;
}

//-----------------------------------------------------------------------------
void HeimdallLedStateCommand::swap(void* pData, ui32 nSizeBytes)
{
  HeimdallLedStateCommand::Data* lpHeader = NULL;

  if (pData && (nSizeBytes >= sizeof(HeimdallLedStateCommand::Data)))
  {
    lpHeader = reinterpret_cast<HeimdallLedStateCommand::Data*>(pData);

    // Nothing to swap.
  }
}


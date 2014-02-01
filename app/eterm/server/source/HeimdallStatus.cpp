#include <stdio.h>
#include "HeimdallStatus.h"
#include "ByteOrder.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace eterm;

//-----------------------------------------------------------------------------
HeimdallStatus::HeimdallStatus()
: GenericPacket(sizeof(HeimdallStatus::Data), 0)
{
}

//-----------------------------------------------------------------------------
HeimdallStatus::Data* const HeimdallStatus::data()
{
  Data* lpHeader = NULL;

  if (isAllocated())
  {
    lpHeader = reinterpret_cast<Data*>(basePtr());
  }

  return lpHeader;
}

//-----------------------------------------------------------------------------
void HeimdallStatus::swap(void* pData, ui32 nSizeBytes)
{
  HeimdallStatus::Data* lpHeader = NULL;

  if (pData && (nSizeBytes >= sizeof(HeimdallStatus::Data)))
  {
    lpHeader = reinterpret_cast<HeimdallStatus::Data*>(pData);

    ByteOrder::NetSwap(lpHeader->opCount);
  }
}


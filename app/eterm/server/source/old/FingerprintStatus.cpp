#include <stdio.h>
#include "FingerprintStatus.h"
#include "ByteOrder.h"

using namespace liber::net;
using namespace liber::netapp;
using namespace eterm;

//-----------------------------------------------------------------------------
FingerprintStatus::FingerprintStatus()
: GenericPacket(sizeof(FingerprintStatus::Data), 0)
{
}

//-----------------------------------------------------------------------------
FingerprintStatus::Data* const FingerprintStatus::data()
{
  Data* lpHeader = NULL;

  if (isAllocated())
  {
    lpHeader = reinterpret_cast<Data*>(basePtr());
  }

  return lpHeader;
}

//-----------------------------------------------------------------------------
void FingerprintStatus::swap(void* pData, ui32 nSizeBytes)
{
  FingerprintStatus::Data* lpHeader = NULL;

  if (pData && (nSizeBytes >= sizeof(FingerprintStatus::Data)))
  {
    lpHeader = reinterpret_cast<FingerprintStatus::Data*>(pData);

    // Nothing to swap.
  }
}


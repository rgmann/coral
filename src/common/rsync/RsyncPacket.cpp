#include <stdio.h>
#include "Log.h"
#include "RsyncPacket.h"
#include "ByteOrder.h"

using namespace liber;
using namespace liber::net;
using namespace liber::netapp;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket()
{
}

//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket(int type, ui32 length, const void* pData)
: GenericPacket(sizeof(RsyncPacket::Data), length)
{
  if (isAllocated())
  {
    data()->type = type;
    data()->length = length;

    if (pData)
    {
      memcpy(data(), pData, length);
    }
  }
}

//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket(int type, const std::string& rData)
: GenericPacket(sizeof(RsyncPacket::Data), rData.size())
{
  if (isAllocated())
  {
    data()->type = type;
    data()->length = rData.size();
    log::debug("RsyncPacket::RsyncPacket - type=%d, length=%d\n", type, rData.size());

    if (rData.size() > 0)
    {
      memcpy(data(), rData.data(), rData.size());
    }
  }
}


//-----------------------------------------------------------------------------
bool RsyncPacket::allocate(const RsyncPacket::Data& rData)
{
  bool lbSuccess = GenericPacket::allocate(sizeof(RsyncPacket::Data), 
                                           rData.length);

  if (lbSuccess)
  {
    data()->type = rData.type;
    data()->length = rData.length;
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
RsyncPacket::Data* const RsyncPacket::data() const
{
  Data* lpHeader = NULL;

  if (isAllocated())
  {
    lpHeader = reinterpret_cast<Data*>(basePtr());
  }

  return lpHeader;
}

//-----------------------------------------------------------------------------
void RsyncPacket::swap(void* pData, ui32 nSizeBytes)
{
  RsyncPacket::Data* lpHeader = NULL;

  if (pData && (nSizeBytes >= sizeof(RsyncPacket::Data)))
  {
    lpHeader = reinterpret_cast<RsyncPacket::Data*>(pData);

    lpHeader->type   = ByteOrder::NetSwap(lpHeader->type);
    lpHeader->length = ByteOrder::NetSwap(lpHeader->length);
  }
}


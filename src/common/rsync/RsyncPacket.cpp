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
      memcpy(dataPtr(), pData, length);
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

    if (rData.size() > 0)
    {
      memcpy(dataPtr(), rData.data(), rData.size());
    }
  }
}

//-----------------------------------------------------------------------------
RsyncPacket::RsyncPacket(int type, const GenericPacket* pPacket)
: GenericPacket(sizeof(RsyncPacket::Data), pPacket->allocatedSize())
{
  if (isAllocated())
  {
    data()->type = type;
    data()->length = pPacket->allocatedSize();

    if (pPacket->allocatedSize() > 0)
    {
      memcpy(dataPtr(), pPacket->basePtr(), pPacket->allocatedSize());
    }
  }
}


//-----------------------------------------------------------------------------
/*bool RsyncPacket::allocate(const RsyncPacket::Data& rData)
{
  bool lbSuccess = GenericPacket::allocate(sizeof(RsyncPacket::Data), 
                                           rData.length);

  if (lbSuccess)
  {
    data()->type = rData.type;
    data()->length = rData.length;
  }

  return lbSuccess;
}*/

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

//-----------------------------------------------------------------------------
bool RsyncPacket::unpack(const void* pData, ui32 nSizeBytes)
{
  bool lbSuccess = inherited::unpack(pData, nSizeBytes);

  if (lbSuccess)
  {
    lbSuccess = (allocatedSize() >= sizeof(RsyncPacket::Data));

    if (lbSuccess)
    {
      mnDataSizeBytes = sizeof(RsyncPacket::Data);
    }
    else
    {
      log::error("RsyncPacket::unpack - Received buffer is too small\n");
    }
  }

  return lbSuccess;
}

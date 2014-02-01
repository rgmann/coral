#include <stdio.h>
#include "NetAppPacket.h"
#include "ByteOrder.h"

using namespace liber::net;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
NetAppPacket::NetAppPacket()
{
}

//-----------------------------------------------------------------------------
NetAppPacket::NetAppPacket(int type, ui32 length)
: GenericPacket(sizeof(NetAppPacket::Data), length)
{
  if (isAllocated())
  {
    data()->type = type;
    data()->length = length;
  }
}

//-----------------------------------------------------------------------------
bool NetAppPacket::allocate(const NetAppPacket::Data& rData)
{
  bool lbSuccess = GenericPacket::allocate(sizeof(NetAppPacket::Data), 
                                           rData.length);

  if (lbSuccess)
  {
    data()->type = rData.type;
    data()->length = rData.length;
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
NetAppPacket::Data* const NetAppPacket::data()
{
  Data* lpHeader = NULL;

  if (isAllocated())
  {
    lpHeader = reinterpret_cast<Data*>(basePtr());
  }

  return lpHeader;
}

//-----------------------------------------------------------------------------
void NetAppPacket::swap(void* pData, ui32 nSizeBytes)
{
  NetAppPacket::Data* lpHeader = NULL;

  if (pData && (nSizeBytes >= sizeof(NetAppPacket::Data)))
  {
    lpHeader = reinterpret_cast<NetAppPacket::Data*>(pData);

    ByteOrder::NetSwap(lpHeader->type);
    ByteOrder::NetSwap(lpHeader->length);
  }
}


#include "ConnectionStatus.h"
#include "ByteOrder.h"

using namespace liber::netapp;
using namespace liber::net;

//-----------------------------------------------------------------------------
KeepalivePacket::KeepalivePacket()
: GenericPacket(sizeof(KeepalivePacket::Data), 0)
{
}

//-----------------------------------------------------------------------------
KeepalivePacket::Data* const KeepalivePacket::data()
{
  Data* lpHeader = NULL;

  if (isAllocated())
  {
    lpHeader = reinterpret_cast<Data*>(basePtr());
  }

  return lpHeader;
}

//-----------------------------------------------------------------------------
void KeepalivePacket::swap(void* pData, ui32 nSizeBytes)
{
  KeepalivePacket::Data* lpHeader = NULL;

  if (pData && (nSizeBytes >= sizeof(KeepalivePacket::Data)))
  {
    lpHeader = reinterpret_cast<KeepalivePacket::Data*>(pData);

    ByteOrder::NetSwap(lpHeader->count);
  }
}

//-----------------------------------------------------------------------------
ConnectionStatus::ConnectionStatus(ui32 nTimeoutMs)
: mnTimeoutMs(nTimeoutMs)
{
  mLastRecvTime.sample();
}

//-----------------------------------------------------------------------------
void ConnectionStatus::setReceiveTimeoutMs(ui32 nTimeoutMs)
{
  mnTimeoutMs = nTimeoutMs;
}

//-----------------------------------------------------------------------------
bool ConnectionStatus::isActive() const
{
  return (Timestamp::Now().diffInMs(mLastRecvTime) < mnTimeoutMs);
}

//-----------------------------------------------------------------------------
void ConnectionStatus::sampleRecvTime()
{
  mLastRecvTime.sample();
}



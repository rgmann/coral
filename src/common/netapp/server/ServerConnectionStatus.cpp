#include "ServerConnectionStatus.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
ServerConnectionStatus::ServerConnectionStatus()
{
  mKeepalive.data()->count = 0;
}

//-----------------------------------------------------------------------------
ServerConnectionStatus::~ServerConnectionStatus()
{
}

//-----------------------------------------------------------------------------
bool ServerConnectionStatus::put(DestinationID destination, const void* pData, ui32 nLength)
{
  bool lbSuccess = false;

  if (pData)
  {
    KeepalivePacket lKeepaliveRequest;

    lbSuccess = lKeepaliveRequest.unpack(pData, nLength);
    if (lbSuccess)
    {
      mLastRecvTime.sample();

      // Send a keepalive acknowledgement.
      mKeepalive.data()->count++;

      // TODO: Convert to new architecture.
      // sendPacket(&mKeepalive);
    }
  }

  return lbSuccess;
}


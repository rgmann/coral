#include "ClientConnectionStatus.h"

using namespace liber::netapp;

// //-----------------------------------------------------------------------------
// ClientConnectionStatus::ClientConnectionStatus()
// {
//   mKeepalive.data()->count = 0;
//   mLastKeepaliveSendTime.sample();
// }

// //-----------------------------------------------------------------------------
// ClientConnectionStatus::~ClientConnectionStatus()
// {
// }

// //-----------------------------------------------------------------------------
// bool ClientConnectionStatus::sendKeepalive()
// {
//   mLastKeepaliveSendTime.sample();
//   mKeepalive.data()->count++;
//   return sendPacket(&mKeepalive);
// }

// //-----------------------------------------------------------------------------
// const Timestamp& ClientConnectionStatus::lastKeepaliveSendTime() const
// {
//   return mLastKeepaliveSendTime;
// }

// //-----------------------------------------------------------------------------
// bool ClientConnectionStatus::put(const char* pData, ui32 nLength)
// {
//   bool lbSuccess = false;

//   if (pData)
//   {
//     KeepalivePacket lKeepaliveRequest;

//     lbSuccess = lKeepaliveRequest.unpack(pData, nLength);
//     if (lbSuccess)
//     {
//       mLastRecvTime.sample();
//     }
//   }

//   return lbSuccess;
// }


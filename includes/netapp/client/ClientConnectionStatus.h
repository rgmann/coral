#ifndef CLIENT_CONNECTION_STATUS_H
#define CLIENT_CONNECTION_STATUS_H

#include "ConnectionStatus.h"

namespace liber {
namespace netapp {

class ClientConnectionStatus : public ConnectionStatus {
public:

  ClientConnectionStatus();
  ~ClientConnectionStatus();

  bool sendKeepalive();

  const Timestamp& lastKeepaliveSendTime() const;

  bool put(const char* pData, ui32 nLength);

private:

  KeepalivePacket mKeepalive;
  Timestamp       mLastKeepaliveSendTime;
};

} // End namespace netapp
} // End namespace liber

#endif // CLIENT_CONNECTION_STATUS_H


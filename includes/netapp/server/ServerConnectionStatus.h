#ifndef SERVER_CONNECTION_STATUS_H
#define SERVER_CONNECTION_STATUS_H

#include "ConnectionStatus.h"

namespace liber {
namespace netapp {

class ServerConnectionStatus : public ConnectionStatus {
public:

  ServerConnectionStatus();
  ~ServerConnectionStatus();

  bool put(DestinationID destination, const void* pData, ui32 nLength);

private:

  KeepalivePacket mKeepalive;
};

} // End namespace netapp
} // End namespace liber

#endif // SERVER_CONNECTION_STATUS_H


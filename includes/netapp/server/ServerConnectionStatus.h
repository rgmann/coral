#ifndef SERVER_CONNECTION_STATUS_H
#define SERVER_CONNECTION_STATUS_H

#include "ConnectionStatus.h"

namespace liber {
namespace netapp {

class ServerConnectionStatus : public ConnectionStatus {
public:

  ServerConnectionStatus();
  ~ServerConnectionStatus();

  bool put(const char* pData, ui32 nLength);

private:

  KeepalivePacket mKeepalive;
};

}}

#endif // SERVER_CONNECTION_STATUS_H


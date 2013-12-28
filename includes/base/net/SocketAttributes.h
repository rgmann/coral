#ifndef SOCKET_ATTRIBUTES_H
#define SOCKET_ATTRIBUTES_H

#include <netinet/in.h>
#include <sys/select.h>
#include "BaseTypes.h"

namespace liber {
namespace net {

enum SocketError {
  SocketOk = 0,
  SocketTimeout,
  SocketInvalid,
  SocketDisconnect,
  ConnectionRefused,
  SocketInterrupted,
  NotConnected
};

struct SocketAttributes {
  int                port;
  int                fd;
  fd_set             set;
  struct sockaddr_in addr;
};

struct SocketStatus {
  SocketError status;     // Return status.
  ui32        byteCount;  // Number of bytes transferred.
};

}}

#endif // SOCKET_ATTRIBUTES_H


#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include "Socket.h"

namespace liber {
namespace net {

class UdpSocket : public Socket {
public:

  UdpSocket(const char* pHost, int nSource, int nDestination);
  virtual ~UdpSocket();

  /**
   *
   */
  void read(SocketStatus& status, char* pData, int nMaxBytes, int nTimeoutMs = Block);

  /**
   *
   */
  void write(SocketStatus& status, const char* pData, int nBytes, int nTimeoutMs = 0);



};

}}

#endif // UDP_SOCKET_H


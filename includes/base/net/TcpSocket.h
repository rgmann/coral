#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include "Socket.h"

namespace liber {
namespace net {

class TcpSocket : public Socket {
public:
	
  TcpSocket();
   
  ~TcpSocket();
	
  /**
   * @ret  int   Returns the number of bytes received
   */
  void read(SocketStatus& status, char* pBuffer, int nBytes, int nTimeoutMs);
	
  int canRecv(int nTimeoutMs);
	
  void write(SocketStatus& status, const char* pBuffer, int nBytes, int nTimeoutMs = 0);
   
  void disconnect();
	
private:
	
   friend class TcpClient;
   friend class TcpServer;
};

}}

#endif // TCP_SOCKET_H

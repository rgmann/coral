#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

//#include <netinet/in.h>
//#include <sys/select.h>
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
  int read(char* pBuffer, int nBytes, int nTimeoutMs);
	
  int canRecv(int nTimeoutMs);
	
  int write(const char* pBuffer, int nBytes, int nTimeoutMs = 0);
   
  //void close();
	
private:
	
/*   int   m_nSocket;
   
   int   m_nPort;
	
   fd_set mFdSet;
   
   struct sockaddr_in mAddr;
  */ 
   friend class TcpClient;
   friend class TcpServer;
};

}}

#endif // TCP_SOCKET_H

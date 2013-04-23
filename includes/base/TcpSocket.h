#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <netinet/in.h>
#include <sys/select.h>

class TcpSocket
{
public:
	
	TcpSocket();
   
   ~TcpSocket();
	
   /**
    * @ret  int   Returns the number of bytes received
    */
	int   recv(char* pBuffer, int nBytes, int nTimeoutMs);
	
	int   canRecv(int nTimeoutMs);
	
	int   send(const char* pBuffer, int nBytes);
   
   void  closeSocket();
	
private:
	
	int   m_nSocket;
   
   int   m_nPort;
	
	fd_set mFdSet;
   
   struct sockaddr_in mAddr;
   
   friend class TcpClient;
   friend class TcpServer;
};

#endif // TCP_SOCKET_H
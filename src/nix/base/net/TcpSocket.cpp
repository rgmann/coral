#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX_SIZE 50

#include "TcpSocket.h"

using namespace liber::net;

//-----------------------------------------------------------------------------
TcpSocket::TcpSocket()
{
//   m_nSocket = -1;
//   FD_ZERO(&mFdSet);
}

//-----------------------------------------------------------------------------
TcpSocket::~TcpSocket()
{
}

//-----------------------------------------------------------------------------
int TcpSocket::read(char* pBuffer, int nBytes, int nTimeoutMs)
{
  int            lnBytesRead = -1;
  struct timeval lTimeout;
  fd_set         lReadSet;
   
  if (mAttributes.fd <= 0)
  {
    printf("TcpSocket::recv: invalid socket\n");
    return -1;
  }
	
  lTimeout.tv_sec = nTimeoutMs / 1000;
  lTimeout.tv_usec = 1000 * (nTimeoutMs - lTimeout.tv_sec * 1000);
   
  FD_ZERO(&lReadSet);
  FD_SET(mAttributes.fd, &lReadSet);
	
  //if (FD_ISSET(mSocket, &mFdSet))
  {
    // Check to see whether data is available.
    // Blocks for the specified timeout period.
    if (select(mAttributes.fd + 1, &lReadSet, NULL, NULL, &lTimeout) <= 0)
    {
      printf("TcpSocket::recv: timeout\n");
      return -1;
    }
		
    lnBytesRead = Socket::read(pBuffer, nBytes, nTimeoutMs);
      
    if (lnBytesRead == -1)
    {
      perror("TcpSocket::recv: -");
    }
  }
	
  return lnBytesRead;
}

//-----------------------------------------------------------------------------
int TcpSocket::write(const char* pBuffer, int nBytes, int nTimeoutMs)
{
  int lnBytesSent = -1;

  if (mAttributes.fd <= 0)
  {
    return -1;
  }
   
   //if (FD_ISSET(m_nSocket, &mFdSet))
   //{
   //	l_nBytesSent = write(m_nSocket, pBuffer, nBytes);
   //l_nBytesSent = send(m_nSocket, pBuffer, nBytes, MSG_NOSIGNAL);
  lnBytesSent = Socket::write(pBuffer, nBytes, nTimeoutMs);
   //}
	
  return lnBytesSent;
}


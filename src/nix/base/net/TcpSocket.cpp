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
}

//-----------------------------------------------------------------------------
TcpSocket::~TcpSocket()
{
  disconnect();
}

//-----------------------------------------------------------------------------
void TcpSocket::read(SocketStatus& status, char* pBuffer, int nBytes, int nTimeoutMs)
{
  struct timeval lTimeout;
  fd_set         lReadSet;
   
  memset(&status, 0, sizeof(status));
  if (mAttributes.fd > 0)
  {
    // Set the timeout.
    lTimeout.tv_sec = nTimeoutMs / 1000;
    lTimeout.tv_usec = 1000 * (nTimeoutMs - lTimeout.tv_sec * 1000);

    FD_ZERO(&lReadSet);
    FD_SET(mAttributes.fd, &lReadSet);

    if (select(mAttributes.fd + 1, &lReadSet, NULL, NULL, &lTimeout) > 0)
    {
      Socket::read(status, pBuffer, nBytes, nTimeoutMs);
    }
    else
    {
      status.status = SocketTimeout;
    }
  }
  else
  {
    printf("TcpSocket::recv: invalid socket\n");
    status.status = SocketInvalid;
  }
}

//-----------------------------------------------------------------------------
void TcpSocket::
write(SocketStatus& status, const char* pBuffer, int nBytes, int nTimeoutMs)
{
  memset(&status, 0, sizeof(status));
  if (mAttributes.fd > 0)
  {
    Socket::write(status, pBuffer, nBytes, nTimeoutMs);
  }
  else
  {
    status.status = SocketInvalid;
  }
}

//------------------------------------------------------------------------------
void TcpSocket::disconnect()
{
  // TCP sockets need to be shutdown.
  if (mAttributes.fd > 0)
  {
    shutdown(mAttributes.fd, SHUT_RDWR);
  }

  Socket::disconnect();
}


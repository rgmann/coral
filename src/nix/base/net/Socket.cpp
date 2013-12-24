#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include "Socket.h"

using namespace liber::net;

//------------------------------------------------------------------------------
Socket::Socket()
{
  mAttributes.fd = -1;
  FD_ZERO(&mAttributes.set); 
}

//------------------------------------------------------------------------------
Socket::~Socket()
{
  disconnect();
}

//------------------------------------------------------------------------------
void Socket::setAttributes(const SocketAttributes& attributes)
{
  mAttributes = attributes;
}

//------------------------------------------------------------------------------
int Socket::read(char* pBuffer, int nBytes, int nTimeoutMs)
{
   return recv(mAttributes.fd, pBuffer, nBytes, 0);
}

//------------------------------------------------------------------------------
int Socket::write(const char* pBuffer, int nBytes, int nTimeoutMs)
{
  int lnBytesSent = -1;

#if defined(__APPLE__) && defined(__MACH__)
  lnBytesSent = send(mAttributes.fd, pBuffer, nBytes, 0);
#elif defined(__linux__)
  lnBytesSent = send(mAttributes.fd, pBuffer, nBytes, MSG_NOSIGNAL);
#endif

  return lnBytesSent;
}

//------------------------------------------------------------------------------
void Socket::disconnect()
{
  if (mAttributes.fd > 0)
  {
    shutdown(mAttributes.fd, SHUT_RDWR);
    close(mAttributes.fd);
    FD_CLR(mAttributes.fd, &mAttributes.set);
    mAttributes.fd = -1;
  }
}


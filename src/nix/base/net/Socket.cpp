#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
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
}

//------------------------------------------------------------------------------
void Socket::setAttributes(const SocketAttributes& attributes)
{
  mAttributes = attributes;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void Socket::
read(SocketStatus& status, char* pBuffer, int nBytes, int nTimeoutMs)
{
  int lRecvStatus = -1;

  memset(&status, 0, sizeof(status));

  lRecvStatus = recv(mAttributes.fd, pBuffer, nBytes, 0);
  if (lRecvStatus < 0)
  {
    perror("Socket::read: ");
    status.status = TranslateErrno(errno);
  }
  // Peer performed an orderly disconnect.
  else if (lRecvStatus == 0)
  {
    status.status = SocketDisconnect;
  }
  else
  {
    status.status = SocketOk;
    status.byteCount = (ui32) lRecvStatus;
  }
}

//------------------------------------------------------------------------------
void Socket::
write(SocketStatus& status, const char* pBuffer, int nBytes, int nTimeoutMs)
{
  int lSendStatus = -1;

  memset(&status, 0, sizeof(status));

#if defined(__APPLE__) && defined(__MACH__)
  lSendStatus = send(mAttributes.fd, pBuffer, nBytes, 0);
#elif defined(__linux__)
  lSendStatus = send(mAttributes.fd, pBuffer, nBytes, MSG_NOSIGNAL);
#endif

  if (lSendStatus < 0)
  {
    perror("Socket::write: ");
    status.status = TranslateErrno(errno);
  }
  else
  {
    status.status = SocketOk;
    status.byteCount = (ui32) lSendStatus;
  }
}

//------------------------------------------------------------------------------
SocketError Socket::TranslateErrno(int err)
{
  SocketError lError = SocketOk;

  if (err == EAGAIN || err == EWOULDBLOCK)
  {
    lError = SocketTimeout;
  }
  else if (err == EBADF || err == ENOTSOCK)
  {
    lError = SocketInvalid;
  }
  else if (err == EINTR)
  {
    lError = SocketInterrupted;
  }
  else if (err == ECONNRESET)
  {
    lError = SocketDisconnect;
  }

  return lError;
}

//------------------------------------------------------------------------------
void Socket::disconnect()
{
  if (mAttributes.fd > 0)
  {
    close(mAttributes.fd);
    FD_CLR(mAttributes.fd, &mAttributes.set);
    mAttributes.fd = -1;
  }
}


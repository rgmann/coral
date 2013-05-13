#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include "SocketHelper.h"

//------------------------------------------------------------------------------
int SocketHelper::read(int fd, char* pBuffer, int nBytes)
{
   return recv(fd, pBuffer, nBytes, 0);
}

//------------------------------------------------------------------------------
int SocketHelper::write(int fd, const char* pBuffer, int nBytes)
{
   int l_nBytesSent = -1;
   
#if defined(__APPLE__) && defined(__MACH__)
   l_nBytesSent = send(fd, pBuffer, nBytes, 0);
#elif defined(__linux__)
   l_nBytesSent = send(fd, pBuffer, nBytes, MSG_NOSIGNAL);
#endif
   
   return l_nBytesSent;
}

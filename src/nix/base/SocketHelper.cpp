#include <sys/socket.h>
#include <sys/types.h>
#include "SocketHelper.h"

//------------------------------------------------------------------------------
int SocketHelper::read(int fd, char* pBuffer, int nBytes)
{
   return recv(fd, pBuffer, nBytes, 0);
}

//------------------------------------------------------------------------------
int SocketHelper::write(int fd, const char* pBuffer, int nBytes)
{
   return send(fd, pBuffer, nBytes, MSG_NOSIGNAL);
}

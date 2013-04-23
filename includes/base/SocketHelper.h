#ifndef SOCKET_HELPER_H
#define SOCKET_HELPER_H

class SocketHelper
{
public:
	
	/**
    * @ret  int   Returns the number of bytes received
    */
	static int read(int fd, char* pBuffer, int nBytes);
	
	static int write(int fd, const char* pBuffer, int nBytes);
};

#endif // SOCKET_HELPER_H
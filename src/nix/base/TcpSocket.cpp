#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX_SIZE 50

#include "SocketHelper.h"
#include "TcpSocket.h"

//-----------------------------------------------------------------------------
TcpSocket::TcpSocket()
{
	m_nSocket = -1;
	FD_ZERO(&mFdSet);
}

//-----------------------------------------------------------------------------
TcpSocket::~TcpSocket()
{
	closeSocket();
}

//-----------------------------------------------------------------------------
int TcpSocket::recv(char* pBuffer, int nBytes, int nTimeoutMs)
{
	int            l_nBytesRead = -1;
	struct timeval	l_tTimeout;
   fd_set         l_ReadSet;
   
   if (m_nSocket <= 0)
   {
      printf("TcpSocket::recv: invalid socket\n");
      return -1;
   }
	
	l_tTimeout.tv_sec = nTimeoutMs / 1000;
	l_tTimeout.tv_usec = 1000 * (nTimeoutMs - l_tTimeout.tv_sec * 1000);
   
   FD_ZERO(&l_ReadSet);
   FD_SET(m_nSocket, &l_ReadSet);
	
	//if (FD_ISSET(mSocket, &mFdSet))
	{
		// Check to see whether data is available.
		// Blocks for the specified timeout period.
		if (select(m_nSocket + 1, &l_ReadSet, NULL, NULL, &l_tTimeout) <= 0)
		{
         //printf("TcpSocket::recv: timeout\n");
			return -1;
		}
		
		//l_nBytesRead = read(m_nSocket, pBuffer, nBytes);
      l_nBytesRead = SocketHelper::read(m_nSocket, pBuffer, nBytes);
      
      if (l_nBytesRead == -1)
      {
         perror("TcpSocket::recv: -");
      }
	}
	
	return l_nBytesRead;
}

//-----------------------------------------------------------------------------
int TcpSocket::send(const char* pBuffer, int nBytes)
{
	int l_nBytesSent = -1;
	
   if (m_nSocket <= 0)
   {
      return -1;
   }
   
	//if (FD_ISSET(m_nSocket, &mFdSet))
	//{
	//	l_nBytesSent = write(m_nSocket, pBuffer, nBytes);
   //l_nBytesSent = send(m_nSocket, pBuffer, nBytes, MSG_NOSIGNAL);
   l_nBytesSent = SocketHelper::write(m_nSocket, pBuffer, nBytes);
	//}
	
	return l_nBytesSent;
}

//-----------------------------------------------------------------------------
void TcpSocket::closeSocket()
{
	if (m_nSocket > 0)
	{
      printf("TcpSocket::closeSocket\n");
      
      shutdown(m_nSocket, SHUT_RDWR);
      
		close(m_nSocket);
      
		FD_CLR(m_nSocket, &mFdSet);
		m_nSocket = -1;
	}
}

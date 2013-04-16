#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>

#define MAX_SIZE 50

#include "TcpSocket.h"
#include "TcpServer.h"

//-----------------------------------------------------------------------------
TcpServer::TcpServer()
{
   FD_ZERO(&m_ServerSet);
   m_fdListenSocket = -1;
   m_nPort = -1;
}

//-----------------------------------------------------------------------------
TcpServer::~TcpServer()
{
	stop();
}

//-----------------------------------------------------------------------------
bool TcpServer::start(int port)
{
   int yes = 1;
	/* master file descriptor list */
    //fd_set master;
	
    /* temp file descriptor list for select() */
    //fd_set read_fds;
	
    /* server address */
    struct sockaddr_in serveraddr;
	
    /* client address */
    //struct sockaddr_in clientaddr;
	
	/* get the listener */
	if((m_fdListenSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		
		printf("Server-socket() error!\r\n");
		return false;
	}
	
	//printf("Server-socket() is OK...\n");
	
	/*"address already in use" error message */
	if(setsockopt(m_fdListenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		printf("Server-setsockopt() error!\r\n");
		return false;
	}
	
	//printf("Server-setsockopt() is OK...\n");
	
	/* bind */
	m_nPort = port;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	serveraddr.sin_port = htons(m_nPort);
	
	memset(&(serveraddr.sin_zero), '\0', 8);
	
	if(bind(m_fdListenSocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
	{
		printf("Server-bind() error!\r\n");
		return false;
	}
	
	printf("Server-bind() is OK...\n");
	
	/* listen */
	if(listen(m_fdListenSocket, 10) == -1)
	{
		printf("Server-listen() error!\r\n");
		return false;
	}
	
	//printf("Server-listen() is OK...\n");
	
	/* add the listener to the master set */
	FD_ZERO(&m_ServerSet);
	FD_SET(m_fdListenSocket, &m_ServerSet);
	
	/* keep track of the biggest file descriptor */
	// mFdMax = mListenSocket; /* so far, it's this one*/
	
	return true;
}

//-----------------------------------------------------------------------------
void TcpServer::stop()
{
	// Can't stop a server that hasn't been started...
	if (m_fdListenSocket <= 0)
	{
		return;
	}
	
	close(m_fdListenSocket);
	FD_CLR(m_fdListenSocket, &m_ServerSet);
	
	m_fdListenSocket	= -1;
	m_nPort			= -1;
}

//-----------------------------------------------------------------------------
TcpSocket* TcpServer::acceptClient(int nTimeoutMs)
{
	struct	timeval locTimeout;
	
	int		locNumReady = 0;
   int      locClientSocketFd = 0;
	
	struct	sockaddr_in clientAddr;
	
	TcpSocket* locpTcpSocket = NULL;
	
	fd_set	locClientSet;  // descriptor set to be monitored
	
	if (m_fdListenSocket <= 0)
	{
		printf("TcpServer::Accept:  Server not started.\r\n");
		return NULL;
	}
	
	locTimeout.tv_sec = nTimeoutMs / 1000;
	locTimeout.tv_usec = 1000 * (nTimeoutMs - locTimeout.tv_sec * 1000);
	
	
	// pass max descriptor and wait indefinitely until data arrives
	locNumReady = select(m_fdListenSocket+1, &m_ServerSet, NULL, NULL, &locTimeout);
	if (locNumReady == 0)
	{
		//printf("TcpServer::Accept - Select timeout\r\n");
		return NULL;
	}
	if (locNumReady < 0)
	{
		printf("TcpServer::Accept - Select error\r\n");
		return NULL;
	}
	
	//printf("Waiting\n");
	
	if(FD_ISSET(m_fdListenSocket, &m_ServerSet)) // new client connection
	{
		socklen_t nAddrLen = sizeof(clientAddr);
		
		locClientSocketFd = accept(m_fdListenSocket, 
                                 (struct sockaddr *)&clientAddr, 
                                 &nAddrLen);
		if (locClientSocketFd == -1)
		{
			perror("TcpServer::Accept - accept error!");
			return NULL;
		}
		
		FD_SET(locClientSocketFd, &locClientSet); /* add to master set */
		
      locpTcpSocket              = new TcpSocket();
		locpTcpSocket->m_nSocket	= locClientSocketFd;
		locpTcpSocket->mFdSet      = locClientSet;
		locpTcpSocket->m_nPort     = m_nPort;
		memcpy(&locpTcpSocket->mAddr, &clientAddr, nAddrLen);
	}
	
	return locpTcpSocket;
}


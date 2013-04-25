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
   int ret = 0;
	/* master file descriptor list */
    //fd_set master;
	
    /* temp file descriptor list for select() */
    //fd_set read_fds;
	
    /* server address */
    struct sockaddr_in serveraddr;
	
    /* client address */
    //struct sockaddr_in clientaddr;
	
	/* get the listener */
   m_fdListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_fdListenSocket < 0)
	{
		perror("socket() failed");
		return false;
	}
	
	//printf("Server-socket() is OK...\n");
	
	/*"address already in use" error message */

   ret = setsockopt(m_fdListenSocket, SOL_SOCKET, SO_REUSEADDR,
                    &yes, sizeof(int));
#if defined(__APPLE__) && defined(__MACH__)
   ret = setsockopt(m_fdListenSocket, SOL_SOCKET, SO_NOSIGPIPE,
                    &yes, sizeof(int));
#endif
   
	if(ret < 0)
	{
		perror("setsockopt() failed");
      close(m_fdListenSocket);
		return false;
	}
	
	//printf("Server-setsockopt() is OK...\n");
	
	/* bind */
	m_nPort = port;
   memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(m_nPort);
	
	memset(&(serveraddr.sin_zero), '\0', 8);
	
   ret = bind(m_fdListenSocket, (struct sockaddr *)&serveraddr,
              sizeof(serveraddr));
	if(ret < 0)
	{
		perror("bind() failed");
      close(m_fdListenSocket);
		return false;
	}
	
	printf("Server-bind() is OK...\n");
	
	/* listen */
   ret = listen(m_fdListenSocket, 10);
	if(ret < 0)
	{
      perror("listen() failed");
      close(m_fdListenSocket);
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
   
   fd_set l_WorkingSet;
	
	TcpSocket* locpTcpSocket = NULL;
	
	fd_set	locClientSet;  // descriptor set to be monitored
	
	if (m_fdListenSocket <= 0)
	{
		printf("TcpServer::Accept:  Server not started.\r\n");
		return NULL;
	}
	
	locTimeout.tv_sec = nTimeoutMs / 1000;
	locTimeout.tv_usec = 1000 * (nTimeoutMs - locTimeout.tv_sec * 1000);
	
	
   memcpy(&l_WorkingSet, &m_ServerSet, sizeof(fd_set));
   
	// pass max descriptor and wait indefinitely until data arrives
	locNumReady = select(m_fdListenSocket+1, &l_WorkingSet,
                        NULL, NULL, &locTimeout);
	if (locNumReady < 0)
	{
		perror("  TcpServer::Accept - Select failed");
		return NULL;
	}
   
	if (locNumReady == 0)
	{
		//printf("TcpServer::Accept - Select timed out\n");
		return NULL;
	}
	
	//printf("Waiting\n");
	
	if(FD_ISSET(m_fdListenSocket, &l_WorkingSet)) // new client connection
	{
      printf("New connection!\n");
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


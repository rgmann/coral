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

using namespace liber::net;

//-----------------------------------------------------------------------------
TcpServer::TcpServer()
{
   FD_ZERO(&mServerSet);
   mListenSocket = -1;
   mnPort = -1;
}

//-----------------------------------------------------------------------------
TcpServer::~TcpServer()
{
  if (mListenSocket > 0)
  {
    stop();
  }
}

//-----------------------------------------------------------------------------
bool TcpServer::start(int port)
{
   int yes = 1;
   int ret = 0;

   struct sockaddr_in serveraddr;
	
   mListenSocket = socket(AF_INET, SOCK_STREAM, 0);
   if(mListenSocket < 0)
   {
      perror("socket() failed");
      return false;
   }
	
   ret = setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEADDR,
                    &yes, sizeof(int));
#if defined(__APPLE__) && defined(__MACH__)
   ret = setsockopt(mListenSocket, SOL_SOCKET, SO_NOSIGPIPE,
                    &yes, sizeof(int));
#endif
   
   if(ret < 0)
   {
      perror("setsockopt() failed");
      close(mListenSocket);
      return false;
   }
	
	
   mnPort = port;
   memset(&serveraddr, 0, sizeof(serveraddr));
   serveraddr.sin_family = AF_INET;
   serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
   serveraddr.sin_port = htons(mnPort);
	
   memset(&(serveraddr.sin_zero), '\0', 8);
	
   ret = bind(mListenSocket, (struct sockaddr *)&serveraddr,
              sizeof(serveraddr));
   if(ret < 0)
   {
      perror("bind() failed");
      close(mListenSocket);
      return false;
   }
	
   printf("Server-bind() is OK...\n");
	
   /* listen */
   ret = listen(mListenSocket, 10);
   if(ret < 0)
   {
      perror("listen() failed");
      close(mListenSocket);
      return false;
   }

   //printf("Server-listen() is OK...\n");
	
   /* add the listener to the master set */
   FD_ZERO(&mServerSet);
   FD_SET(mListenSocket, &mServerSet);
	
   /* keep track of the biggest file descriptor */
   // mFdMax = mListenSocket; /* so far, it's this one*/
	
  return true;
}

//-----------------------------------------------------------------------------
void TcpServer::stop()
{
  // Can't stop a server that hasn't been started...
  if (mListenSocket <= 0)
  {
    return;
  }

  printf("TcpServer::close started\n");
  close(mListenSocket);
  FD_CLR(mListenSocket, &mServerSet);
  printf("TcpServer::close complete\n");
	
  mListenSocket = -1;
  mnPort = -1;
}

//-----------------------------------------------------------------------------
TcpSocket* TcpServer::acceptClient(int nTimeoutMs)
{
   SocketAttributes attributes;
   struct timeval locTimeout;
	
   int locNumReady = 0;
	
   fd_set lWorkingSet;
	
   TcpSocket* lpTcpSocket = NULL;
	

   if (mListenSocket <= 0)
   {
      printf("TcpServer::Accept:  Server not started.\r\n");
      return NULL;
   }
	
   locTimeout.tv_sec = nTimeoutMs / 1000;
   locTimeout.tv_usec = 1000 * (nTimeoutMs - locTimeout.tv_sec * 1000);
	
	
   memcpy(&lWorkingSet, &mServerSet, sizeof(fd_set));
   
   // pass max descriptor and wait indefinitely until data arrives
   locNumReady = select(mListenSocket+1, &lWorkingSet,
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
	
   if(FD_ISSET(mListenSocket, &lWorkingSet)) // new client connection
   {
      printf("New connection!\n");
      socklen_t nAddrLen = sizeof(attributes.addr);
		
      attributes.fd = accept(mListenSocket, 
                             (struct sockaddr *)&attributes.addr, 
                              &nAddrLen);
      if (attributes.fd == -1)
      {
         perror("TcpServer::Accept - accept error!");
         return NULL;
      }

      // Get the existing socket settings.
      /*int opts = 0;
      if ((opts = fcntl(attributes.fd, F_GETFL)) < 0 )
      {
        perror("TcpServer::Accept: Failed to get opts - ");
        close(attributes.fd);
        return NULL;
      }*/

      // Setting socket to non-block mode
      /*if (fcntl(attributes.fd, opts | O_NONBLOCK) < 0 )
      {
        perror("TcpServer::Accept: Failed set NONBLOCK - ");
        close(attributes.fd);
        return NULL;
      }*/

      FD_SET(attributes.fd, &attributes.set); /* add to master set */
      attributes.port = mnPort;

      lpTcpSocket = new TcpSocket();
      lpTcpSocket->setAttributes(attributes);
   }
	
   return lpTcpSocket;
}

//-----------------------------------------------------------------------------
bool TcpServer::acceptClient(TcpSocket& rSocket, int nTimeoutMs)
{
   SocketAttributes attributes;
   struct timeval locTimeout;
	
   int locNumReady = 0;
	
   fd_set lWorkingSet;
	

   if (mListenSocket <= 0)
   {
      printf("TcpServer::Accept:  Server not started.\r\n");
      return false;
   }
	
   locTimeout.tv_sec = nTimeoutMs / 1000;
   locTimeout.tv_usec = 1000 * (nTimeoutMs - locTimeout.tv_sec * 1000);
	
	
   memcpy(&lWorkingSet, &mServerSet, sizeof(fd_set));
   
   // pass max descriptor and wait indefinitely until data arrives
   locNumReady = select(mListenSocket+1, &lWorkingSet,
                        NULL, NULL, &locTimeout);
   if (locNumReady < 0)
   {
      perror("  TcpServer::Accept - Select failed");
      return false;
   }
   
   if (locNumReady == 0)
   {
      //printf("TcpServer::Accept - Select timed out\n");
      return false;
   }
	
   //printf("Waiting\n");
	
   if(FD_ISSET(mListenSocket, &lWorkingSet)) // new client connection
   {
      printf("New connection!\n");
      socklen_t nAddrLen = sizeof(attributes.addr);
		
      attributes.fd = accept(mListenSocket, 
                             (struct sockaddr *)&attributes.addr, 
                              &nAddrLen);
      if (attributes.fd == -1)
      {
         perror("TcpServer::Accept - accept error!");
         return false;
      }

      FD_SET(attributes.fd, &attributes.set); /* add to master set */
      attributes.port = mnPort;

      rSocket.setAttributes(attributes);
   }
	
   return rSocket.isConnected();
}


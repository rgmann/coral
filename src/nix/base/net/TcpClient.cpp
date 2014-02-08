#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "TcpSocket.h"
#include "TcpClient.h"

using namespace liber::net;

//----------------------------------------------------------
TcpClient::TcpClient()
{
}

//----------------------------------------------------------
TcpClient::~TcpClient()
{
}

//-----------------------------------------------------------------------------
TcpSocket* TcpClient::Connect(const char* host, int port, int nTimeoutMs)
{
  TcpSocket* lpSocket = NULL;
  SocketAttributes attributes;
   
  int opts = 0;
   
  int   locRet = 0;
   
  struct   timeval locTimeout;
	
  fd_set   rset, wset;

  attributes.port = port;
	
  locTimeout.tv_sec = nTimeoutMs / 1000;
  locTimeout.tv_usec = 1000 * (nTimeoutMs - locTimeout.tv_sec * 1000);
  printf("Host: %s\r\n", host);
	
  // Create the client socket.
  if((attributes.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Error : Could not create socket \n");
    return NULL;
  }
	
  memset(&attributes.addr, 0, sizeof(attributes.addr)); 
	
  attributes.addr.sin_family = AF_INET;
  attributes.addr.sin_port = htons(attributes.port); 
	
  if(inet_pton(AF_INET, host, &attributes.addr.sin_addr) <= 0)
  {
    perror("inet_pton error occured");
    return NULL;
  } 
	
  // Get the existing socket settings.
  if ((opts = fcntl(attributes.fd, F_GETFL)) < 0 )
  {
    perror("TcpClient::Connect: Failed to get opts - ");
    close(attributes.fd);
    return NULL;
  }
	   
  // Setting socket to non-block mode
  if (fcntl(attributes.fd, opts | O_NONBLOCK) < 0 )
  {
    perror("TcpClient::Connect: Failed set NONBLOCK - ");
    close(attributes.fd);
    return NULL;
  }

  locRet = connect(attributes.fd, 
                   (struct sockaddr *)&attributes.addr, 
                   sizeof(attributes.addr));
	
  if ((locRet == -1) && (errno != EINPROGRESS))
  {
    perror("TcpClient::Connect: Failed set connect - ");
    close(attributes.fd);
    return NULL;
  }
  else if (locRet == -1)
  {
    FD_ZERO(&rset);
    FD_SET(attributes.fd, &rset);
    wset = rset;

    int nDescCount = select(attributes.fd + 1, &rset, &wset, NULL, &locTimeout);
    if (nDescCount == 0)
    {
      close(attributes.fd);
      return NULL;
    }

    if (FD_ISSET(attributes.fd, &rset) || FD_ISSET(attributes.fd, &wset))
    {
      int result;
      socklen_t result_len = sizeof(result);

      if (getsockopt(attributes.fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0)
      {
        close(attributes.fd);
        return NULL;     // error, fail somehow, close socket
      }
    }
    else
    {
      printf("select error: sockfd not set");
      close(attributes.fd);
      return NULL;
    }

  }

  fcntl(attributes.fd, F_SETFL, opts);
	
  lpSocket = new TcpSocket();
  lpSocket->setAttributes(attributes);
 
  return lpSocket;
}

//-----------------------------------------------------------------------------
bool TcpClient::
Connect(TcpSocket& rSocket, const char* host, int port, int nTimeoutMs)
{
  SocketAttributes attributes;
   
  int opts = 0;
   
  int   locRet = 0;
   
  struct   timeval locTimeout;
	
  fd_set   rset, wset;

  attributes.port = port;
	
  locTimeout.tv_sec = nTimeoutMs / 1000;
  locTimeout.tv_usec = 1000 * (nTimeoutMs - locTimeout.tv_sec * 1000);
  printf("Host: %s\r\n", host);
	
  // Create the client socket.
  if((attributes.fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Error : Could not create socket \n");
    return false;
  }
	
  memset(&attributes.addr, 0, sizeof(attributes.addr)); 
	
  attributes.addr.sin_family = AF_INET;
  attributes.addr.sin_port = htons(attributes.port); 
	
  if(inet_pton(AF_INET, host, &attributes.addr.sin_addr) <= 0)
  {
    perror("inet_pton error occured");
    return false;
  } 
	
  // Get the existing socket settings.
  if ((opts = fcntl(attributes.fd, F_GETFL)) < 0 )
  {
    perror("TcpClient::Connect: Failed to get opts - ");
    close(attributes.fd);
    return false;
  }
	   
  // Setting socket to non-block mode
  if (fcntl(attributes.fd, opts | O_NONBLOCK) < 0 )
  {
    perror("TcpClient::Connect: Failed set NONBLOCK - ");
    close(attributes.fd);
    return false;
  }

  locRet = connect(attributes.fd, 
                   (struct sockaddr *)&attributes.addr, 
                   sizeof(attributes.addr));
	
  if ((locRet == -1) && (errno != EINPROGRESS))
  {
    perror("TcpClient::Connect: Failed set connect - ");
    close(attributes.fd);
    return false;
  }
  else if (locRet == -1)
  {
    FD_ZERO(&rset);
    FD_SET(attributes.fd, &rset);
    wset = rset;

    int nDescCount = select(attributes.fd + 1, &rset, &wset, NULL, &locTimeout);
    if (nDescCount == 0)
    {
      close(attributes.fd);
      return false;
    }

    if (FD_ISSET(attributes.fd, &rset) || FD_ISSET(attributes.fd, &wset))
    {
      int result;
      socklen_t result_len = sizeof(result);

      if (getsockopt(attributes.fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0)
      {
        close(attributes.fd);
        return false;     // error, fail somehow, close socket
      }
    }
    else
    {
      printf("select error: sockfd not set");
      close(attributes.fd);
      return false;
    }

  }

  fcntl(attributes.fd, F_SETFL, opts);
	
  rSocket.setAttributes(attributes);
 
  return rSocket.isConnected();
}


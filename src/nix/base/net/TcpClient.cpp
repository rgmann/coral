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
/*TcpSocket* TcpClient::Connect(const char* host, int port, int nTimeoutMs)
{
   int l_nClientSocket = -1;
   
   int opts = 0;
   
   int   locRet = 0;
   
   struct sockaddr_in   locServAddr;
   
   struct   timeval locTimeout;
	
   TcpSocket*			l_pSocket = NULL;
   
   fd_set   rset, wset;
	
   locTimeout.tv_sec = nTimeoutMs / 1000;
   locTimeout.tv_usec = 1000 * (nTimeoutMs - locTimeout.tv_sec * 1000);
   printf("Host: %s\r\n", host);
	
   // Create the client socket.
	if((l_nClientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      printf("\n Error : Could not create socket \n");
      return NULL;
   }
	
   memset(&locServAddr, 0, sizeof(locServAddr)); 
	
   locServAddr.sin_family = AF_INET;
   locServAddr.sin_port = htons(port); 
	
   if(inet_pton(AF_INET, host, &locServAddr.sin_addr) <= 0)
   {
      perror("inet_pton error occured");
      return NULL;
   } 
	
   // Get the existing socket settings.
   if ((opts = fcntl(l_nClientSocket, F_GETFL)) < 0 )
   {
      perror("TcpClient::Connect: Failed to get opts - ");
      close(l_nClientSocket);
      return NULL;
   }
	   
	// Setting socket to non-block mode
	if (fcntl(l_nClientSocket, opts | O_NONBLOCK) < 0 )
	{
      perror("TcpClient::Connect: Failed set NONBLOCK - ");
		close(l_nClientSocket);
		return NULL;
	}
	
	locRet = connect(l_nClientSocket, 
                    (struct sockaddr *)&locServAddr, 
                    sizeof(locServAddr));
	
   if ((locRet == -1) && (errno != EINPROGRESS))
	{
      perror("TcpClient::Connect: Failed set connect - ");
		close(l_nClientSocket);
		return NULL;
	}
	else if (locRet == -1)
	{
      int n = 0;
		FD_ZERO(&rset);
		FD_SET(l_nClientSocket, &rset);
		wset = rset;
		
		if ((n = select(l_nClientSocket+1, &rset, &wset, NULL, &locTimeout)) == 0)
		{
			close(l_nClientSocket);          // timeout 
			//errno = ETIMEDOUT;
			return NULL;
		}
		
		if (FD_ISSET(l_nClientSocket, &rset) || FD_ISSET(l_nClientSocket, &wset))
		{
			int result;
			socklen_t result_len = sizeof(result);
         
			if (getsockopt(l_nClientSocket, 
						   SOL_SOCKET, SO_ERROR, 
						   &result, &result_len) < 0)
			{
				close(l_nClientSocket);
				return NULL;     // error, fail somehow, close socket
			}
		}
		else
		{
			printf("select error: sockfd not set");
			close(l_nClientSocket);
			return NULL;
		}
			
	}

	fcntl(l_nClientSocket, F_SETFL, opts);  // restore file status flags 
	
	// We can now create the socket object.
	l_pSocket = new TcpSocket();
	l_pSocket->m_nSocket = l_nClientSocket;
   //l_pSocket->mFdSet = rset;
   l_pSocket->m_nPort	= port;
   memcpy(&l_pSocket->mAddr, &locServAddr, sizeof(locServAddr));
	   
	return l_pSocket;
}
*/

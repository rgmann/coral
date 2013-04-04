#include "TcpServer.h"
#include "TcpSocket.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
   TcpSocket*  locpSocket = NULL;
   TcpServer   locServer;
   
   char  l_sString[] = "hello world!";
   
   printf("Server test\r\n");
   
   if (!locServer.start(5000))
   {
      printf("Failed to start server\r\n");
      return 1;
   }
   
   locpSocket = locServer.acceptClient(10000);
   
   if (locpSocket == NULL)
   {
      printf("Accept failed\r\n");
      return 1;
   }
   printf("Accepted connection\r\n");
   locpSocket->send(l_sString, strlen(l_sString));
   
   
   sleep(10);
   locServer.stop();
   
   locpSocket->closeSocket();
   printf("Stopped server\r\n");
   
   delete locpSocket;
   locpSocket = NULL;
   
   return 0;
}

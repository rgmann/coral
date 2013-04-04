#include "ArgParser.h"
#include "TcpClient.h"
#include "TcpSocket.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
   ArgParser args;
   TcpSocket*  locpSocket = NULL;
   
   std::string l_sHostName;
   int         l_nPort = 5000;
   
   char buffer[256];
   int l_nBytes = 0;
   
   printf("Client test\r\n");
   
   //args.addArg("h", ArgParser::ArgOption, ArgParser::ArgRequired, 
//               ArgParser::ArgString, "host", "HostName", "Host name/address");
   args.addArg("name: HostName, primary: h, alt: host, type: opt, \
               vtype: string, required, desc: Host name/address");
   //args.addArg("p", ArgParser::ArgOption, ArgParser::ArgRequired, 
//               ArgParser::ArgString, "port", "Port", "Set port number");
   args.addArg("name: Port, primary: p, alt: port, type: opt, \
               vtype: int, required, desc: Set port number");
   
   if (!args.parse((const char**)argv, argc))
   {
      args.printArgErrors(true);
      return 1;
   }
   
   args.getArgVal(l_sHostName, Argument::ArgName, "HostName");
   args.getArgVal(l_nPort, Argument::ArgName, "Port");
   
   locpSocket = TcpClient::Connect(l_sHostName.c_str(), l_nPort, 2000);
   
   if (locpSocket == NULL)
   {
      printf("Failed to connect\r\n");
      return 1;
   }
   printf("Connected to %s on port number %d\n", l_sHostName.c_str(), l_nPort);
   
   memset(buffer, 0, 256);
   l_nBytes = locpSocket->recv(buffer, 256, 5000);
   
   printf("Received %d bytes: %s\n", l_nBytes, buffer);
   
   locpSocket->closeSocket();
   
   delete locpSocket;
   locpSocket = NULL;
   
   return 0;
}

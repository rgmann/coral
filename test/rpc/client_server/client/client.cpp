#include <stdio.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include "ArgParser.h"
#include "TcpSocket.h"
#include "TcpClient.h"
#include "Thread.h"
#include "BinarySem.h"
#include "CalculatorClientStub.h"
#include "DatabaseClientStub.h"

struct RpcThreadArgs
{
   RpcClient *pClient;
   BinarySem *pStartSem;
};

bool  gbStopCommanded = false;

std::string gsHostName;
int         gnPort = 5000;
int         gnUserId = 0;

void sighandler(int s)
{
   printf("Got signal\n");
   gbStopCommanded = true;
}

void rxThread(ThreadArg* pArg);
i32  receiveRpcResponse(RpcPacket** pResp, TcpSocket* pSocket);
void calculator_run(RpcClient& client);
void database_run(RpcClient& client);

int main(int argc, char *argv[])
{
   ArgParser args;
   Thread*  mpRxThread = NULL;

   RpcClient lClient;
   BinarySem lStartSem;

   RpcThreadArgs lRpcArgs;
   lRpcArgs.pClient = &lClient;
   lRpcArgs.pStartSem = &lStartSem;
   
   struct sigaction sigIntHandler;
      
   printf("Client:\n\n");
   
   args.addArg("name: HostName, primary: h, alt: host, type: opt, \
               vtype: string, required, desc: Host name/address");
   
   args.addArg("name: Port, primary: p, alt: port, type: opt, \
               vtype: int, required, desc: Set port number");
   
   if (!args.parse((const char**)argv, argc))
   {
      args.printArgErrors(true);
      return 1;
   }
   
   args.getArgVal(gsHostName, Argument::ArgName, "HostName");
   args.getArgVal(gnPort, Argument::ArgName, "Port");
   
   sigIntHandler.sa_handler = sighandler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;
   sigaction(SIGINT, &sigIntHandler, NULL);
   
   mpRxThread = Thread::Create(rxThread, &lRpcArgs);
   
   if (mpRxThread == NULL)
   {
      printf("Failed to start RX thread.\n");
      return 1;
   }

   lStartSem.take();

   calculator_run(lClient);
   database_run(lClient);
   
   gbStopCommanded = true;
   
   printf("Stopping\n");
   
   if (mpRxThread)
   {
      mpRxThread->stop();
      mpRxThread->join();
   }
   
   return 0;
}

//-----------------------------------------------------------------------------
void calculator_run(RpcClient &client)
{
   CalculatorClientStub calculator(client);
   CalculatorClientStub calcb(client);

   int lnResult = 0;

   try {
      calcb.add(10, 4);
      calcb.div(10, 2);
   } catch (RpcError& e) {
      std::cout << "Error" << std::endl;
   }

   try {
      lnResult = calculator.add(5, 4);
      std::cout << "5 + 4 = " << lnResult << std::endl;
   } catch (RpcError& e) {
      std::cout << "Add failed!" << std::endl;
   }
   try {
      lnResult = calculator.sub(5, 4);
      std::cout << "5 - 4 = " << lnResult << std::endl;
   } catch (RpcError& e) {
      std::cout << "Sub failed!" << std::endl;
   }
   try {
      lnResult = calculator.mul(5, 4);
      std::cout << "5 * 4 = " << lnResult << std::endl;
   } catch (RpcError& e) {
      std::cout << "Mul failed!" << std::endl;
   }
   try {
      lnResult = calculator.div(10, 5);
      std::cout << "10 / 5 = " << lnResult << std::endl;
   } catch (RpcError& e) {
      std::cout << "Div failed!" << std::endl;
   }
   try {
      lnResult = calculator.getNumOps();
      std::cout << "# of ops = " << lnResult << std::endl;
   } catch (RpcError& e) {
      std::cout << "getNumOps failed" << std::endl;
   }

   try {
      lnResult = calcb.getNumOps();
      std::cout << "calcb # of ops = " << lnResult << std::endl;
   } catch (RpcError& e) {
      std::cout << "calcb.getNumOps() failed" << std::endl;
   }
}

//-----------------------------------------------------------------------------
void database_run(RpcClient& client)
{
   DatabaseClientStub database(client);

   UserAttributes ted("Ted", 65, "brown");
   UserAttributes gloria("Gloria", 83, "grey");
   UserAttributes paul("Paul", 50, "blue");

   try {
      if (!database.create("Nugent", ted))
      {
         std::cout << "Failed to create Ted Nugent." << std::endl;
      }
   } catch (RpcError& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      if (!database.create("Elmsworth", gloria))
      {
         std::cout << "Failed to create Gloria Elmsworth." << std::endl;
      }
   } catch (RpcError& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      if (!database.create("Newman", paul))
      {
         std::cout << "Failed to create Paul Newman." << std::endl;
      }
   } catch (RpcError& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      UserAttributes a;
      if (!database.read("Newman", a))
      {
         std::cout << "Failed to read Paul Newman." << std::endl;
      }
      else {
         std::string lastName = "";
         i32 age = 0;
         std::string eyeColor = "";

         a.get(lastName, age, eyeColor);

         std::cout << "Paul's info:" << std::endl;
         std::cout << "  lastname:" << lastName << std::endl;
         std::cout << "  age:" << age << std::endl;
         std::cout << "  eye color:" << eyeColor << std::endl;
      }
   } catch (RpcError& e) {
      std::cout << "database.read failed: " << e.toString() << std::endl;
   }
}

//------------------------------------------------------------------------------
void rxThread(ThreadArg* pArg)
{
   TcpSocket*  lpSocket = NULL;
   
   RpcThreadArgs* lpRpcArgs = reinterpret_cast<RpcThreadArgs*>(pArg->pUserData);
   
   // Connect to server
   lpSocket = TcpClient::Connect(gsHostName.c_str(), gnPort, 2000);
   
   if (lpSocket == NULL)
   {
      printf("Failed to connect to server\n");
      while (!pArg->stopSignalled()) sleep(1);
   }

   lpRpcArgs->pStartSem->give();
   
   while (!pArg->stopSignalled())
   {
      RpcPacket* lpRpcPacket = NULL;
      
      // Check for a new msg in the tx queue
      if (lpRpcArgs->pClient->popPacket(&lpRpcPacket))
      {
         char*      pMsgData = NULL;
         ui32       msgLen = 0;
         
         lpRpcPacket->pack((void**)&pMsgData, msgLen);
     
         printf("Sending packet!\n");    
         lpSocket->send(pMsgData, msgLen);
         
         delete[] pMsgData;
         pMsgData = NULL;

         delete lpRpcPacket;
         lpRpcPacket = NULL;
      }

      if (receiveRpcResponse(&lpRpcPacket, lpSocket) > 0)
      {
         if (lpRpcPacket)
         {
            lpRpcArgs->pClient->processPacket(lpRpcPacket);
            delete lpRpcPacket;
            lpRpcPacket = NULL;
         }
      }
   }
   
   lpSocket->closeSocket();
}

//------------------------------------------------------------------------------
i32 receiveRpcResponse(RpcPacket** pResp, TcpSocket* pSocket)
{
   RpcPacket::Data  header;
   //bool             lbSuccess = false;
   i32              lnBytesRecvd = 0;
   char*            lpPkt = NULL;
   ui32             lnDataLen     = 0;

   
   // Attempt to receive a chat packet header
   lnBytesRecvd = pSocket->recv((char*)&header,
                                sizeof(RpcPacket::Data),
                                1000);
   
   if (lnBytesRecvd == -1)
   {
      return -1;
   }
   
   //if (header.marker != ChatPacketHdr::marker) return false;
   
   // Determine how much data is included in the packet.
   lnDataLen = header.length;
   lpPkt = new char[sizeof(RpcPacket::Data) + lnDataLen];
   
   if (!lpPkt) return false;
   
   // Copy the previously read header into the allocated packet.
   memcpy(lpPkt, &header, sizeof(RpcPacket::Data));
   
   // Receive the rest of the packet directly into the allocated buffer.
   lnBytesRecvd = pSocket->recv(lpPkt + sizeof(RpcPacket::Data),
                                 lnDataLen, 1000);
   
   if (lnBytesRecvd == lnDataLen)
   {
      *pResp = new RpcPacket();
      (*pResp)->unpack(lpPkt, sizeof(RpcPacket::Data) + lnDataLen);
   }
   
   if (lpPkt)
   {
      delete lpPkt;
   }
   
   return lnBytesRecvd;
}

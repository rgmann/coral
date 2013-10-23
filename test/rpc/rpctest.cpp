#include <iostream>
#include "Thread.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "RpcServer.h"
#include "CalculatorClientStub.h"
#include "CalculatorServerStub.h"

struct RpcThreadArgs
{
   RpcClient *pClient;
   BinarySem *pStartSem;
};

void routingThread(ThreadArg* pArg);

void run(RpcClient &client)
{
   CalculatorClientStub calculator(client);
   CalculatorClientStub calcb(client);
   
   int lnResult = 0;
   
   calcb.add(10, 4, lnResult);
   calcb.div(10, 2, lnResult);
   
   if (calculator.add(5, 4, lnResult)) {
      std::cout << "5 + 4 = " << lnResult << std::endl;
   }
   if (calculator.sub(5, 4, lnResult)) {
      std::cout << "5 - 4 = " << lnResult << std::endl;
   }
   if (calculator.mul(5, 4, lnResult)) {
      std::cout << "5 * 4 = " << lnResult << std::endl;
   }
   if (calculator.div(10, 5, lnResult)) {
      std::cout << "10 / 5 = " << lnResult << std::endl;
   }
   if (calculator.getNumOps(lnResult)) {
      std::cout << "# of ops = " << lnResult << std::endl;
   }
   
   if (calcb.getNumOps(lnResult)) {
      std::cout << "calcb # of ops = " << lnResult << std::endl;
   }
}

int main()
{
   Thread*  mpRoutingThread = NULL;
   BinarySem lStartSem;
   RpcClient client;
   RpcThreadArgs lArgs;
   
   lArgs.pClient = &client;
   lArgs.pStartSem = &lStartSem;


   mpRoutingThread = Thread::Create(routingThread, &lArgs);
   
   lStartSem.take();
   std::cout << "RPC test:" << std::endl;

   run(client);
   
   if (mpRoutingThread)
   {
      mpRoutingThread->stop();
      mpRoutingThread->join();
      
      delete mpRoutingThread;
      mpRoutingThread = NULL;
   }
   
   return 0;
}

void routingThread(ThreadArg* pArg)
{
   RpcThreadArgs *lpArgs = NULL;
   //RpcClient *pClient = NULL;
   RpcPacket *pPacket = NULL;
   RpcServer server;
   CalculatorServerStub calculator;
   
   server.registerResource(&calculator);
   
   lpArgs = reinterpret_cast<RpcThreadArgs*>(pArg->pUserData);
   
   lpArgs->pStartSem->give();
   
   while (!pArg->stopSignalled())
   {
      if (lpArgs->pClient->popPacket(&pPacket))
      {
         server.processPacket(pPacket);
         delete pPacket;
         pPacket = NULL;
      }

      if (server.popPacket(&pPacket))
      {
         lpArgs->pClient->processPacket(pPacket);
         delete pPacket;
         pPacket = NULL;
      }
   }   
}


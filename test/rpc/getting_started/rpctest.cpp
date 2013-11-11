#include <iostream>
#include "Thread.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "RpcServer.h"
#include "CalculatorClientStub.h"
#include "CalculatorServerStub.h"
#include "DatabaseClientStub.h"
#include "DatabaseServerStub.h"

struct RpcThreadArgs
{
   RpcClient *pClient;
   BinarySem *pStartSem;
};

void routingThread(ThreadArg* pArg);

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

   //calculator_run(client);
   database_run(client);
   
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
   DatabaseServerStub database;
   
   server.registerResource(&calculator);
   server.registerResource(&database);
   
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


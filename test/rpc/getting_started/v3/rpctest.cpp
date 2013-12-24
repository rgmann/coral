#include <iostream>
#include "Thread.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "RpcServer.h"
#include "CalculatorClientStub.h"
#include "CalculatorServerStub.h"
#include "DatabaseClientStub.h"
#include "DatabaseServerStub.h"

using namespace liber::rpc;
using namespace liber::netapp;
using namespace rpc_getting_started;

struct RpcThreadArgs
{
   Queue<NetAppPacket*>* pQueue;
   liber::rpc::RpcClient *pClient;
   BinarySem *pStartSem;
};

void routingThread(ThreadArg* pArg);

void tor_hook(CalculatorWrapper* pInst, void* pUserData)
{
   std::string userMsg;

   if (pUserData)
   {
      userMsg = (char*)pUserData;
      std::cout << userMsg << ": " << std::endl;
   }
}

void calculator_run(liber::rpc::RpcClient &client)
{
   CalculatorClientStub calculator(client);
   CalculatorClientStub calcb(client);
   
   Parameters params;
   Result     result;

   try {
      params.add_values(5);
      params.add_values(4);
      calcb.add(params, result);
      calcb.div(params, result);
   } catch (RpcException& e) {
      std::cout << "Error" << std::endl;
   }
   
   try {
      params.clear_values();
      result.Clear();
      params.add_values(5);
      params.add_values(4);
      calculator.add(params, result);
      std::cout << "5 + 4 = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "Add failed!" << std::endl;
   }
   try {
      params.clear_values();
      result.Clear();
      params.add_values(5);
      params.add_values(4);
      calculator.sub(params, result);
      std::cout << "5 - 4 = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "Sub failed!" << std::endl;
   }
   try {
      params.clear_values();
      result.Clear();
      params.add_values(5);
      params.add_values(4);
      calculator.mul(params, result);
      std::cout << "5 * 4 = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "Mul failed!" << std::endl;
   }
   try {
      params.clear_values();
      result.Clear();
      params.add_values(10);
      params.add_values(5);
      calculator.div(params, result);
      std::cout << "10 / 5 = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "Div failed:" << std::endl
                << e.trace()
                << std::endl;
   }
   try {
      params.clear_values();
      result.Clear();
      calculator.getNumOps(params, result);
      std::cout << "# of ops = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "getNumOps failed" << std::endl;
   }
   
   try {
      params.clear_values();
      result.Clear();
      calcb.getNumOps(params, result);
      std::cout << "calcb # of ops = " << result.result() << std::endl;
   } catch (RpcException& e) {
      std::cout << "calcb.getNumOps() failed" << std::endl;
   }
}

void database_run(liber::rpc::RpcClient& client)
{
   DatabaseClientStub database(client);

   DbStatus status;
   UserAttributes attributes;
   UserName name;

   try {
      name.set_first("Ted");
      name.set_last("Nugent");
      attributes.set_allocated_name(new UserName(name));
      attributes.set_age(65);
      attributes.set_eye_color("brown");
      database.create(attributes, status);

      if (!status.success())
      {
         std::cout << "Failed to create Ted Nugent." << std::endl;
      }
   } catch (RpcException& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      status.Clear();
      name.set_first("Gloria");
      name.set_last("Elmsworth");
      attributes.set_allocated_name(new UserName(name));
      attributes.set_age(83);
      attributes.set_eye_color("grey");
      database.create(attributes, status);

      if (!status.success())
      {
         std::cout << "Failed to create Gloria Elmsworth." << std::endl;
      }
   } catch (RpcException& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      status.Clear();
      name.set_first("Paul");
      name.set_last("Newman");
      attributes.set_allocated_name(new UserName(name));
      attributes.set_age(50);
      attributes.set_eye_color("blue");
      database.create(attributes, status);

      if (!status.success())
      {
         std::cout << "Failed to create Paul Newman." << std::endl;
      }
   } catch (RpcException& e) {
      std::cout << "database.create failed!" << std::endl;
   }

   try {
      UserAttributesList attrList;
      name.Clear();
      attributes.Clear();

      name.set_first("Paul");
      name.set_last("Newman");
      database.read(name, attrList);
      if (attrList.users_size() == 0)
      {
         std::cout << "Failed to read Paul Newman." << std::endl;
      }
      else {
         for (int ind = 0; ind < attrList.users_size(); ind++)
         {
            std::cout << "Paul's info:" << std::endl;
            std::cout << "  lastname:" << attrList.users(ind).name().last() << std::endl;
            std::cout << "  age:" << attrList.users(ind).age() << std::endl;
            std::cout << "  eye color:" << attrList.users(ind).eye_color() << std::endl;
         }
      }
   } catch (RpcException& e) {
      std::cout << "database.read failed: " << e.toString() << std::endl;
   }
}


int main()
{
   Thread*  mpRoutingThread = NULL;
   BinarySem lStartSem;

   Queue<NetAppPacket*> pktQueue;
   pktQueue.initialize();

   liber::rpc::RpcClient client;
   client.setId(1);
   client.setOutputQueue(&pktQueue);

   RpcThreadArgs lArgs;
   
   lArgs.pQueue = &pktQueue;
   lArgs.pClient = &client;
   lArgs.pStartSem = &lStartSem;


   mpRoutingThread = Thread::Create(routingThread, &lArgs);
   
   lStartSem.take();
   std::cout << "RPC test:" << std::endl;

   calculator_run(client);
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
   liber::rpc::RpcServer server;
   CalculatorServerStub calculator;
   DatabaseServerStub database;
   Queue<NetAppPacket*> serverOutQueue;

   NetAppPacket* lpNetAppPacket = NULL;

   serverOutQueue.initialize();
   server.setId(1);
   server.setOutputQueue(&serverOutQueue);
   server.registerResource(&calculator);
   server.registerResource(&database);


   calculator.registerCtorHook(tor_hook, (void*)"ctor_hook");
   calculator.registerDtorHook(tor_hook, (void*)"dtor_hook");
   
   lpArgs = reinterpret_cast<RpcThreadArgs*>(pArg->pUserData);
   
   lpArgs->pStartSem->give();
   
   while (!pArg->stopSignalled())
   {
      if (lpArgs->pQueue->pop(lpNetAppPacket, 50))
      {
         RpcPacket* lpRpcPacket = new RpcPacket();
         lpRpcPacket->unpack((char*)lpNetAppPacket->dataPtr(), lpNetAppPacket->data()->length);
         //server.processPacket(lpRpcPacket);
         server.put((char*)lpRpcPacket->basePtr(), lpRpcPacket->allocatedSize());
         delete lpRpcPacket;
         lpRpcPacket = NULL;
         delete lpNetAppPacket;
         lpNetAppPacket = NULL;
      }

      if (serverOutQueue.pop(lpNetAppPacket, 50))
      {
         RpcPacket* lpRpcPacket = new RpcPacket();
         lpRpcPacket->unpack((char*)lpNetAppPacket->dataPtr(), lpNetAppPacket->data()->length);
         lpArgs->pClient->put((char*)lpRpcPacket->basePtr(), lpRpcPacket->allocatedSize());
         delete lpRpcPacket;
         lpRpcPacket = NULL;
         delete lpNetAppPacket;
         lpNetAppPacket = NULL;
      }
   }   
}


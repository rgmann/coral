#include <iostream>
#include "Thread.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "RpcServer.h"
#include "SearchServiceClientStub.h"
#include "SearchServiceServerStub.h"
#include "AsyncRpcSupervisor.h"
#include "Log.h"

//#define SYNC_RPC

using namespace liber::rpc;
using namespace liber::netapp;
using namespace tutorial;

struct RpcThreadArgs
{
   Queue<NetAppPacket*>* pQueue;
   liber::rpc::RpcClient *pClient;
   BinarySem *pStartSem;
};

void routingThread(ThreadArg* pArg);

void tor_hook(SearchServiceWrapper* pInst, void* pUserData)
{
   std::string userMsg;

   if (pUserData)
   {
      userMsg = (char*)pUserData;
      std::cout << userMsg << ": " << std::endl;
   }
}

void search_run(liber::rpc::RpcClient &client)
{
  SearchServiceClientStub service(client);
   
  Person request;
  Person_PhoneNumber response;

  request.set_name("Ned Stark");
  request.set_id(10);

  try {
     service.Search(request, response);
  } catch (RpcException& e) {
     liber::log::status(e.trace().c_str());
  }
}


class SearchSupervisor : public liber::rpc::AsyncRpcSupervisor {
public:

  void callback()
  {
    liber::log::status("async rpc callback\n");
    mSem.give();
  }

  BinarySem mSem;
};

void async_search_run(liber::rpc::RpcClient& client)
{
  SearchServiceClientStub service(client);
  SearchSupervisor supervisor;

  Person request;
  Person_PhoneNumber response;

  request.set_name("Ned Stark");
  request.set_id(10);

  try {
     service.Search(request, response, &supervisor);
  } catch (RpcException& e) {
     liber::log::status(e.trace().c_str());
  }

  supervisor.mSem.take(10000);

  if (supervisor.failed())
  {
    liber::log::status(supervisor.exception().trace().c_str());
  }
  else
  {
    liber::log::status("async rpc success\n");
  }
}

int main()
{
  liber::log::level(liber::log::Debug);
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

#ifdef SYNC_RPC
   search_run(client);
#else
   async_search_run(client);
#endif

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
   SearchServiceServerStub searchService;
   Queue<NetAppPacket*> serverOutQueue;

   NetAppPacket* lpNetAppPacket = NULL;

   serverOutQueue.initialize();
   server.setId(1);
   server.setOutputQueue(&serverOutQueue);
   server.registerResource(&searchService);

   searchService.registerCtorHook(tor_hook, (void*)"ctor_hook");
   searchService.registerDtorHook(tor_hook, (void*)"dtor_hook");
   
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


#include <iostream>
#include "Thread.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "RpcServer.h"
#include "SearchServiceClientStub.h"
#include "SearchServiceServerStub.h"
#include "AsyncRpcSupervisor.h"
#include "Log.h"
#include "IntraRouter.h"

#define RPC_ID     2
#define SYNC_RPC

using namespace liber::rpc;
using namespace liber::netapp;
using namespace tutorial;

void tor_hook(SearchServiceWrapper* pInst, void* pUserData)
{
   std::string userMsg;

   if (pUserData)
   {
      userMsg = (char*)pUserData;
      liber::log::status("%s: ", userMsg.c_str());
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
  liber::log::level(liber::log::Verbose);
  liber::log::options(0);

  IntraRouter localRouter;
  IntraRouter remoteRouter;

  localRouter.setCounterpart(&remoteRouter);
  remoteRouter.setCounterpart(&localRouter);

  localRouter.launch();
  remoteRouter.launch();


  liber::rpc::RpcClient client;

  liber::rpc::RpcServer server;
  SearchServiceServerStub searchService;
  searchService.registerCtorHook(tor_hook, (void*)"ctor_hook");
  searchService.registerDtorHook(tor_hook, (void*)"dtor_hook");
  server.registerResource(&searchService);

  if (localRouter.addSubscriber(RPC_ID, &client) &&
      remoteRouter.addSubscriber(RPC_ID, &server))
  {
    liber::log::status("RPC test:\n");

#ifdef SYNC_RPC
    search_run(client);
#else
    async_search_run(client);
#endif
  }

  localRouter.cancel(true);
  remoteRouter.cancel(true);

  liber::log::flush();
  return 0;
}


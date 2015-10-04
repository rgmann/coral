#include <iostream>
#include "RpcClient.h"
#include "RpcServer.h"
#include "SearchServiceClientStub.h"
#include "SearchServiceServerStub.h"
#include "AsyncRpcSupervisor.h"
#include "Log.h"
#include "IntraRouter.h"

#define RPC_ID     2
// #define SYNC_RPC

using namespace liber::rpc;
using namespace liber::netapp;
using namespace tutorial;


class PersonDatabase {
public:

  PersonDatabase() {
    database_.insert( std::make_pair( "Ned Stark", "303-555-3456" ) );
    database_.insert( std::make_pair( "Mr. Mars", "303-555-8775" ) );
  }

  bool get_by_name( const std::string& name, std::string& number ) const {
    PhoneBook::const_iterator iterator = database_.find( name );
    bool found_entry = ( iterator != database_.end() );
    if ( found_entry ) {
      number = iterator->second;
    }
    return found_entry;
  }

private:

  typedef std::map<std::string,std::string> PhoneBook;
  PhoneBook database_;
};


void search_run(liber::rpc::RpcClient &client)
{
  SearchServiceClientStub service(client);
   
  Person request;
  Person_PhoneNumber response;

  request.set_name("Ned Stark");
  request.set_id(10);

  try {
     service.Search(request, response);

     liber::log::status("%s's phone number is %s\n", request.name().c_str(), response.number().c_str());
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

  if ( supervisor.failed() )
  {
    liber::log::status(supervisor.exception().trace().c_str());
  }
  else
  {
    liber::log::status(
      "%s's phone number is %s\n",
      request.name().c_str(),
      response.number().c_str());
  }
}

class SearchAction : public tutorial::SearchServiceSearchAction {
public:

  SearchAction( PersonDatabase& database )
    : database_( database ){}

protected:

  virtual void Search(
   const Person& request, Person_PhoneNumber& response, liber::rpc::RpcException& e)
  {
    std::string phone_number;

    if ( database_.get_by_name( request.name(), phone_number ) )
    {
      response.set_number( phone_number );

      e.id = NoException;
    }
  }

  PersonDatabase& database_;
};

int main()
{
  liber::log::level(liber::log::Verbose);
  liber::log::options(0);

  PersonDatabase person_database;

  IntraRouter localRouter;
  IntraRouter remoteRouter;
  liber::log::status("local = %p, remote=%p\n", &localRouter, &remoteRouter);

  localRouter.setCounterpart(&remoteRouter);
  remoteRouter.setCounterpart(&localRouter);

  localRouter.launch();
  remoteRouter.launch();


  liber::rpc::RpcClient client( RPC_ID );

  liber::rpc::RpcServer server( RPC_ID );
  
  SearchServiceServerStub searchService;

  SearchAction search_action( person_database );

  server.registerResource(&searchService);
  searchService.addAction( &search_action );

  if (localRouter.subscribe(RPC_ID, &client) &&
      remoteRouter.subscribe(RPC_ID, &server))
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


// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 

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

using namespace coral::rpc;
using namespace coral::netapp;
using namespace coral::thread;
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


void search_run(coral::rpc::RpcClient &client)
{
   SearchServiceClientStub service(client);

   Person request;
   Person_PhoneNumber response;

   request.set_name("Ned Stark");
   request.set_id(10);

   try {
      service.Search(request, response);

      coral::log::status(
         "%s's phone number is %s\n",
         request.name().c_str(),
         response.number().c_str() );

   } catch (RpcException& e) {

     coral::log::status(e.trace().c_str());

   }
}


class SearchSupervisor : public coral::rpc::AsyncRpcSupervisor {
public:

   void callback()
   {
      coral::log::status("async rpc callback\n");
      mSem.give();
   }

   BinarySem mSem;
};

void async_search_run(coral::rpc::RpcClient& client)
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
      coral::log::status(e.trace().c_str());
   }

   supervisor.mSem.take(10000);

   if ( supervisor.failed() )
   {
      coral::log::status(supervisor.exception().trace().c_str());
   }
   else
   {
      coral::log::status(
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
      const Person& request, Person_PhoneNumber& response, coral::rpc::RpcException& e)
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
   coral::log::level( coral::log::Verbose );
   coral::log::options(0);

   PersonDatabase person_database;

   IntraRouter localRouter;
   IntraRouter remoteRouter;

   localRouter.setCounterpart( &remoteRouter );
   remoteRouter.setCounterpart( &localRouter );

   localRouter.launch();
   remoteRouter.launch();


   coral::rpc::RpcClient client( RPC_ID );
   coral::rpc::RpcServer server( RPC_ID );

   SearchServiceServerStub searchService;

   SearchAction search_action( person_database );

   server.registerResource( &searchService );
   searchService.addAction( &search_action );

   if ( localRouter.subscribe( RPC_ID, &client ) &&
        remoteRouter.subscribe( RPC_ID, &server ) )
   {
      coral::log::status("RPC test:\n");

#ifdef SYNC_RPC
      search_run( client );
#else
      async_search_run( client );
#endif
   }

   localRouter.cancel(true);
   remoteRouter.cancel(true);

   coral::log::flush();
   return 0;
}


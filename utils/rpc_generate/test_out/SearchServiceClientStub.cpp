// %% license-end-token %%

// WARNING: Auto-generated code. Do not modify.
#include "SearchServiceClientStub.h"

using namespace liber::rpc;
using namespace tutorial;

//-----------------------------------------------------------------------------
SearchServiceClientStub::SearchServiceClientStub( RpcClient &client )
   : RpcClientResource( client, "SearchService" )
{
}

//-----------------------------------------------------------------------------
SearchServiceClientStub::~SearchServiceClientStub()
{
}

//-----------------------------------------------------------------------------
void SearchServiceClientStub::Search(
   const Person&    request,
   Person_PhoneNumber&         response,
   AsyncRpcSupervisor* supervisor )
{
   if ( call( "Search", request, response, supervisor ) == false )
   {
      throw getLastError();
   }
}

//-----------------------------------------------------------------------------
void SearchServiceClientStub::Add(
   const Person&    request,
   Person_PhoneNumber&         response,
   AsyncRpcSupervisor* supervisor )
{
   if ( call( "Add", request, response, supervisor ) == false )
   {
      throw getLastError();
   }
}



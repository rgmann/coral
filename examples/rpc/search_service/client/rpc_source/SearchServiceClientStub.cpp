#include "SearchServiceClientStub.h"

using namespace coral::rpc;
using namespace tutorial;

//-----------------------------------------------------------------------------
SearchServiceClientStub::
SearchServiceClientStub(RpcClient &client)
: RpcClientResource(client, "SearchService")
{
}

//-----------------------------------------------------------------------------
SearchServiceClientStub::
~SearchServiceClientStub()
{
}

//-----------------------------------------------------------------------------
void SearchServiceClientStub::
Search(const Person& request, Person_PhoneNumber& response, AsyncRpcSupervisor* pSupervisor)
 throw (RpcException)
{
  if (!call("Search", request, response, pSupervisor))
  {
    throw getLastError();
  }
}



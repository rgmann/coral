#include "SearchServiceClientStub.h"

using namespace liber::rpc;
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
Search(const Person& request, Person_PhoneNumber& response)
 throw (RpcException)
{
  if (!call("Search", request, response))
  {
    throw getLastError();
  }
}



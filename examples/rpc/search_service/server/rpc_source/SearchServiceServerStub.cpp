#include "SearchServiceServerStub.h"

using namespace coral::rpc;
using namespace tutorial;

//-----------------------------------------------------------------------------
SearchServiceServerStub::SearchServiceServerStub()
: RpcServerResource("SearchService")
{
}

//-----------------------------------------------------------------------------
SearchServiceServerStub::~SearchServiceServerStub()
{
}

//-----------------------------------------------------------------------------
void SearchServiceServerStub::registerActions()
{
  addAction( &default_search_action_ );
}


#include "SearchServiceServerStub.h"

using namespace liber::rpc;
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
  addAction( &default_Search_action_ );
  addAction( &default_Add_action_ );
}


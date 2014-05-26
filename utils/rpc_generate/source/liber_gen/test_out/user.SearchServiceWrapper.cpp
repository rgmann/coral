#include "SearchServiceWrapper.h"

using namespace liber::rpc;
using namespace tutorial;

//-----------------------------------------------------------------------------
bool SearchServiceWrapper::initialize(const std::string& params)
{
  bool lbSuccess = true;

  // TODO: Perform instance initialization.

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool SearchServiceWrapper::destroy(const std::string& params)
{
  bool lbSuccess = true;

  // TODO: Destroy instance.

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void SearchServiceWrapper::
Search(const Person& request, Person_PhoneNumber& response, RpcException& e)
{
  // TODO: Implement action here and remove the following two lines.
  e.id = UnimplementedAction;
  e.message = "SearchServiceWrapper::Search has not been implemented.";
}


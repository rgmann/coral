#include "tutorial.pb.h"
#include "SearchServiceWrapper.h"

using namespace liber::rpc;
using namespace tutorial;

//-----------------------------------------------------------------------------
SearchServiceWrapper::SearchServiceWrapper()
: InstanceWrapper()
{
}

//-----------------------------------------------------------------------------
SearchServiceWrapper::~SearchServiceWrapper()
{
}

//-----------------------------------------------------------------------------
void SearchServiceWrapper::DELEGATE(Search)
{
  SearchServiceWrapper* lpInst = dynamic_cast<SearchServiceWrapper*>(pInst);
  Person requestMessage;
  Person_PhoneNumber responseMessage;

  e.pushFrame(TraceFrame("SearchServiceWrapper", "Search",
             __FILE__, __LINE__));

  requestMessage.ParseFromString(request);
  if (!requestMessage.IsInitialized())
  {
    e.id = MissingParameters;
    e.message = "Parameters not initialized";
    return;
  }

  lpInst->Search(requestMessage, responseMessage, e);
  responseMessage.SerializeToString(&response);

  e.popFrame();
}


#include "Log.h"
#include "tutorial.pb.h"
#include "SearchServiceWrapper.h"

using namespace liber::rpc;
using namespace tutorial;

SearchServiceSearchAction::SearchServiceSearchAction()
   : liber::rpc::RpcServiceAction( "Search" )
{
}

void SearchServiceSearchAction::operator() ( const std::string& request, std::string& response, RpcException& e )
{
   Person requestMessage;
   Person_PhoneNumber responseMessage;

   e.pushFrame(TraceFrame("SearchServiceWrapper", "Search",
             __FILE__, __LINE__));

   requestMessage.ParseFromString(request);

   if ( requestMessage.IsInitialized() )
   {
      Search( requestMessage, responseMessage, e );
      responseMessage.SerializeToString(&response);

      e.popFrame();
   }
   else
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
   }
}

void SearchServiceSearchAction::Search(
   const Person& request, Person_PhoneNumber& response, liber::rpc::RpcException& e)
{
   e.id = UnimplementedAction;
   e.message = "SearchServiceWrapper::Search has not been implemented.";
}

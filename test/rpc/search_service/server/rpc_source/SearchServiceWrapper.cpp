#include "Log.h"
#include "tutorial.pb.h"
#include "SearchServiceWrapper.h"

using namespace liber::rpc;
using namespace tutorial;


void SearchServiceSearchAction::operator() ( const std::string& request, std::string& response, RpcException& e )
{
   Person requestMessage;
   Person_PhoneNumber responseMessage;

   // liber::log::status("SearchServiceSearchAction::operator\n");
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
   // TODO: Implement action here and remove the following two lines.
   liber::log::status("SearchServiceSearchAction::operator\n");
   e.id = UnimplementedAction;
   e.message = "SearchServiceWrapper::Search has not been implemented.";
}

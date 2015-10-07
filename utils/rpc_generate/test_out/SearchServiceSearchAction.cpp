// %% license-end-token %%

// WARNING: Auto-generated code. Do not modify.
#include "tutorial.pb.h"
#include "SearchServiceSearchAction.h"

using namespace liber::rpc;
using namespace tutorial;

//-----------------------------------------------------------------------------
SearchServiceSearchAction::SearchServiceSearchAction()
   : liber::rpc::RpcServiceAction( "Search" )
{
}

//-----------------------------------------------------------------------------
void SearchServiceSearchAction::operator() (
   const std::string& request, std::string& response, RpcException& e )
{
   Person request_message;
   Person_PhoneNumber response_message;

   e.pushFrame( TraceFrame(
      "SearchServiceSearchAction",
      "Search",
      __FILE__,
      __LINE__
   ));

   request_message.ParseFromString( request );
   if ( request_message.IsInitialized() )
   {
      Search( request_message, response_message, e );
      response_message.SerializeToString( &response );
      e.popFrame();
   }
   else
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
   }
}

//-----------------------------------------------------------------------------
void SearchServiceSearchAction::Search(
   const Person& request,
   Person_PhoneNumber& response,
   liber::rpc::RpcException& e )
{
   e.id = UnimplementedAction;
   e.message = "SearchServiceSearchAction has not been implemented.";
}

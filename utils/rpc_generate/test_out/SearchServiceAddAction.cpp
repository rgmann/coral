#include "tutorial.h"
#include "SearchServiceServiceAddAction.h"

//-----------------------------------------------------------------------------
SearchServiceAddAction::SearchServiceAddAction()
   : liber::rpc::RpcServiceAction( "Search" )
{
}

//-----------------------------------------------------------------------------
void SearchServiceAddAction::operator() (
   const std::string& request, std::string& response, RpcException& e )
{
   Person request_message;
   Person_PhoneNumber response_message;

   e.pushFrame( TraceFrame(
      "SearchServiceAddAction",
      "Add",
      __FILE__,
      __LINE__
   ));

   request_message.ParseFromString( request );
   if ( request_message.IsInitialized() )
   {
      Add( request_message, response_message, e );
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
void SearchServiceAddAction::Search(
   const Person& request,
   Person_PhoneNumber& response,
   liber::rpc::RpcException& e )
{
   e.id = UnimplementedAction;
   e.message = "SearchServiceAddAction has not been implemented.";
}

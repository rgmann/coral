#include "AsyncRpcSupervisor.h"
#include "Log.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::concurrency;

//----------------------------------------------------------------------------
AsyncRpcSupervisor::AsyncRpcSupervisor()
   : RpcSupervisor()
   , IThread( "AsyncRpcSupervisor" )
   , cancelled_( false )
   , marshalled_call_( NULL )
   , response_message_ptr_( NULL )
   , timeout_ms_( 0 )
{
}

//----------------------------------------------------------------------------
AsyncRpcSupervisor::~AsyncRpcSupervisor()
{
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::isBusy()
{
  boost::mutex::scoped_lock guard( call_lock_ );
  return ( marshalled_call_ != NULL );
}

//----------------------------------------------------------------------------
void AsyncRpcSupervisor::cancel()
{
   boost::mutex::scoped_lock guard( call_lock_ );

   if ( marshalled_call_ )
   {
      marshalled_call_->cancel();
   }
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::cancelled() const
{
  return cancelled_;
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::reset()
{
   bool supervisor_busy = isBusy();

   if ( supervisor_busy == false )
   {
      boost::mutex::scoped_lock guard( call_lock_ );

      cancelled_            = false;
      marshalled_call_      = NULL;
      response_message_ptr_ = NULL;

      exception_.reset();
   }

   return ( supervisor_busy == false );
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::failed() const
{
  return ( exception_.id != NoException );
}

//----------------------------------------------------------------------------
bool AsyncRpcSupervisor::invoke(
   RpcClient&        rpc_client,
   const RpcObject&  request,
   PbMessage*        response_message_ptr,
   int               timeout_ms )
{
   bool success = false;

   exception_.pushFrame( TraceFrame(
      "AsyncRpcSupervisor",
      "invoke",
      __FILE__,
      __LINE__
   ));

   log::status("AsyncRpcSupervisor::invoke\n");
   if ( isBusy() == false )
   {
      timeout_ms_           = timeout_ms;
      response_message_ptr_ = response_message_ptr;

      // Send the marshalled RPC to the RpcClient.
      marshalled_call_ = rpc_client.invokeRpc( request );
      log::status("AsyncRpcSupervisor::invoke\n");

      if ( marshalled_call_ != NULL )
      {
         success = launch();
      }
   }

  exception_.popFrame();

  return success;
}

//----------------------------------------------------------------------------
void AsyncRpcSupervisor::run( const bool& bShutdown )
{
   exception_.pushFrame( TraceFrame(
      "AsyncRpcSupervisor",
      "run",
      __FILE__,
      __LINE__
   ));

   if ( marshalled_call_->wait( timeout_ms_ ) )
   {
      if ( marshalled_call_->cancelled() )
      {
         exception_.reporter = RpcException::Client;
         exception_.id       = RpcCallTimeout;
         exception_.message  = "Call was cancelled before a response was received.";
      }
      else
      {
         marshalled_call_->getResult( response_object_ );

         if ( response_object_.exception().id == NoException )
         {
            if (response_message_ptr_)
            {
               response_object_.getParams( *response_message_ptr_ );
            }
         }
         else
         {
            exception_.pushTrace( response_object_.exception() );
         }
      }
   }
   else
   {
      exception_.reporter = RpcException::Client;
      exception_.id       = RpcCallTimeout;
      exception_.message  = "Time out elapsed waiting for resource response.";
   }

   marshalled_call_->dispose();

   {
      boost::mutex::scoped_lock guard( call_lock_ );

      cancelled_       = marshalled_call_->cancelled();
      marshalled_call_ = NULL;
   }

   exception_.popFrame();

   callback();
}


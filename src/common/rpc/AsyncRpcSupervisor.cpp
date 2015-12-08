// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#include "AsyncRpcSupervisor.h"
#include "Log.h"

using namespace coral;
using namespace coral::rpc;
using namespace coral::thread;

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
bool AsyncRpcSupervisor::isBusy() const
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
  return ( exception_.id != kNoException );
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

   if ( isBusy() == false )
   {
      timeout_ms_           = timeout_ms;
      response_message_ptr_ = response_message_ptr;

      // Send the marshalled RPC to the RpcClient.
      marshalled_call_ = rpc_client.invokeRpc( request );

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
      if ( marshalled_call_->canceled() )
      {
         exception_.reporter = RpcException::kClient;
         exception_.id       = kRpcCallTimeout;
         exception_.message  = "Call was cancelled before a response was received.";
      }
      else
      {
         response_object_ = marshalled_call_->getResponse();

         if ( response_object_.exception().id == kNoException )
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
      exception_.reporter = RpcException::kClient;
      exception_.id       = kRpcCallTimeout;
      exception_.message  = "Time out elapsed waiting for resource response.";
   }

   marshalled_call_->dispose();

   {
      boost::mutex::scoped_lock guard( call_lock_ );

      cancelled_       = marshalled_call_->canceled();
      marshalled_call_ = NULL;
   }

   exception_.popFrame();

   callback();
}


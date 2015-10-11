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



#include <iostream>
#include "RpcServer.h"
#include "Log.h"

using namespace liber;
using namespace liber::rpc;
using namespace liber::netapp;

//------------------------------------------------------------------------------
RpcServer::RpcServer( DestinationID client_destination_id )
   : client_destination_id_( client_destination_id )
{
}

//------------------------------------------------------------------------------
bool RpcServer::registerResource( RpcServerResource* resource_ptr )
{
   bool register_success = false;

   if ( resource_ptr )
   {
      if ( resources_.count( resource_ptr->getName() ) == 0 )
      {
         std::pair<ResourceMap::iterator,bool> insert_status;

         insert_status = resources_.insert( std::make_pair(
            resource_ptr->getName(),
            resource_ptr
         ));

         register_success = insert_status.second;

         if ( register_success )
         {
            resource_ptr->registerActions();
         }
      }
   }

   return register_success;
}

//------------------------------------------------------------------------------
bool RpcServer::processPacket( const RpcPacket* packet_ptr )
{
   bool process_success = false;

   RpcObject input_object;

   if ( packet_ptr && packet_ptr->getObject( input_object ) )
   {
      RpcObject output_object;
      
      RpcServerResource* resource_ptr = getResource( input_object );

      input_object.exception().pushFrame( TraceFrame(
         "RpcServer",
         "processPacket",
         __FILE__,
         __LINE__
      ));

      if ( resource_ptr )
      {
         process_success = resource_ptr->unmarshall(
            input_object, output_object );
      }
      else
      {
         input_object.exception().reporter = RpcException::Server;
         input_object.exception().id       = UnknownResource;

         input_object.getResponse( output_object );
      }

      input_object.exception().popFrame();
                   
      sendObject( output_object );
   }
   else
   {
      log::error(
         "RpcServer::processPacket - "
         "Failed to deserialize RpcObject\n" );
   }

   return process_success;
}

//------------------------------------------------------------------------------
bool RpcServer::put( DestinationID destination, const void* data_ptr, ui32 length )
{
   bool route_success = false;
   RpcPacket packet;

   if ( data_ptr )
   {
      if ( packet.allocate( data_ptr, length ) )
      {
         route_success = processPacket( &packet );
      }
      else
      {
         log::error("RpcServer::put: Fail to unpack packet of size %u\n", length );
      }
   }

   return route_success;
}

//------------------------------------------------------------------------------
RpcServerResource* RpcServer::getResource( const RpcObject& object )
{
   RpcServerResource* resource_ptr = NULL;

   ResourceMap::iterator resource_iterator =
      resources_.find( object.callInfo().resource );

   if ( resource_iterator != resources_.end() )
   {
      resource_ptr = resource_iterator->second;
   }

   return resource_ptr;
}

//------------------------------------------------------------------------------
void RpcServer::sendObject( const RpcObject &object )
{
   RpcPacket* packet_ptr = new RpcPacket( object );

   if ( packet_ptr && packet_ptr->isAllocated() )
   {
      sendTo( client_destination_id_, packet_ptr );
   }
   else
   {
      delete packet_ptr;
   }
}



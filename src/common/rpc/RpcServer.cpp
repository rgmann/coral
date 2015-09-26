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
         log::status("RpcServer::processPacket: Found resource\n");

         process_success = resource_ptr->unmarshall(
            input_object, output_object );
      }
      else
      {
         log::status("RpcServer::processPacket: Resource not found\n");
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
      if ( packet.unpack( data_ptr, length ) )
      {
         log::status("RpcServer::put: "
            "Processing request - packet_size=%d, data_size=%d\n", length, packet.data()->length );
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



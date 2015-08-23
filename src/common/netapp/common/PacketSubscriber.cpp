#include <string.h>
#include "Log.h"
#include "GenericPacket.h"
#include "PacketReceiver.h"
#include "PacketSubscriber.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
PacketSubscriber::PacketSubscriber()
{
}

//-----------------------------------------------------------------------------
PacketSubscriber::~PacketSubscriber()
{
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::registerRouter(
   DestinationID destination_id,
   PacketReceiver* receiver_ptr,
   SubscriberMode mode )
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );

   bool success = false;

   if ( routers_.count( destination_id ) == 0 )
   {
      RegisteredRouter router_info;

      router_info.destination_id = destination_id;
      router_info.mode = mode;
      router_info.receiver_ptr = receiver_ptr;

      PacketReceiver* receiver_override_ptr = setReceiver( router_info );
      if ( receiver_override_ptr )
      {
         router_info.receiver_ptr = receiver_override_ptr;
      }

      routers_.insert( std::make_pair( destination_id, router_info ) );

      success = true;
   }

   return success;
}

//-----------------------------------------------------------------------------
PacketReceiver* PacketSubscriber::setReceiver( RegisteredRouter& router_info )
{
   return router_info.receiver_ptr;
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::unregisterRouter( DestinationID destination_id )
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );

   bool success = false;

   RegisteredRouterMap::iterator router_iterator = routers_.find( destination_id );
   if ( router_iterator != routers_.end() )
   {
      routers_.erase( router_iterator );
      success = true;
   }

   return success;
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::isSubscribed() const
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );
   return ( routers_.size() > 0 );
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::sendTo( DestinationID destination_id, GenericPacket* packet_ptr )
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );

   bool success = false;

   RegisteredRouterMap::iterator router_iterator = routers_.begin();
   for (; router_iterator != routers_.end(); ++router_iterator )
   {
      PacketContainer* container_ptr = new PacketContainer( destination_id, packet_ptr );

      RegisteredRouter& router_info = router_iterator->second;
      if ( router_info.mode == kSubscriberModeReadWrite )
      {
         PacketReceiver* receiver_ptr = router_info.receiver_ptr;
         if ( receiver_ptr != NULL )
         {
            success = receiver_ptr->send( container_ptr );
         }
      }

      delete container_ptr;
   }

  return success;
}


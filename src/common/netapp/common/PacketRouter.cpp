#include "Log.h"
#include "PacketSubscriber.h"
#include "PacketReceiver.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
PacketRouter::PacketRouter(PacketReceiver* pReceiver)
: receiver_ptr_(pReceiver)
{
}

//-----------------------------------------------------------------------------
PacketRouter::~PacketRouter()
{
   DestinationIterator destination_iterator = destination_table_.begin();
   for (; destination_iterator != destination_table_.end(); ++destination_iterator )
   {
      if ( destination_iterator->second )
      {
         delete destination_iterator->second;
      }
   }
   destination_table_.clear();

   receiver_ptr_ = NULL;
}

//-----------------------------------------------------------------------------
bool PacketRouter::subscribe(
  DestinationID            destination_id,
  PacketSubscriber* subscriber_ptr,
  SubscriberMode          mode
)
{
   bool add_success = false;

   if ( subscriber_ptr )
   {
      boost::mutex::scoped_lock guard( table_lock_ );

      Destination* destination_ptr = NULL;
      DestinationIterator destination_iterator = destination_table_.find( destination_id );

      // If the channel does not exist, add the new channel
      if ( destination_iterator == destination_table_.end() )
      {
         destination_ptr = new Destination( destination_id );

         destination_table_.insert( std::make_pair( destination_id, destination_ptr ) );
      }
      else
      {
         destination_ptr = destination_iterator->second;
      }

      {
         boost::mutex::scoped_lock destination_guard( destination_ptr->lock() );
         
         // Verify that the subcriber is not already subscribed to this channel.
         SubscriberList& subscribers = destination_ptr->subscribers();
         SubscriberIterator subscriber_iterator = subscribers.begin();
         for (; subscriber_iterator != subscribers.end(); ++subscriber_iterator )
         {
            if ( *subscriber_iterator == subscriber_ptr )
            {
               break;
            }
         }

         // If we reached the end, then the subscriber is not already registered.
         if ( subscriber_iterator == subscribers.end() )
         {
            if ( subscriber_ptr->registerRouter( destination_id, receiver_ptr_, mode ) )
            {
               subscribers.push_back( subscriber_ptr );

               add_success = true;
            }
            else
            {
               log::error("PacketRouter::subscribe: Failed to register router\n");
            }
         }
      }
   }

   return add_success;
}

//-----------------------------------------------------------------------------
bool PacketRouter::unsubscribe( DestinationID destination_id, PacketSubscriber* subscriber_ptr )
{
   boost::mutex::scoped_lock guard( table_lock_ );

   bool remove_success = false;
   DestinationIterator destination_iterator = destination_table_.find( destination_id );

   if ( destination_iterator != destination_table_.end() )
   {
      Destination* destination_ptr = destination_iterator->second;

      boost::mutex::scoped_lock destination_guard( destination_ptr->lock() );

      SubscriberList& subscribers = destination_ptr->subscribers();
      SubscriberIterator subscriber_iterator = subscribers.begin();
      for (; subscriber_iterator != subscribers.end(); ++subscriber_iterator )
      {
         if ( *subscriber_iterator == subscriber_ptr )
         {
            subscribers.erase( subscriber_iterator );
            remove_success = subscriber_ptr->unregisterRouter( destination_id );
            break;
         }
      }
   }

   return remove_success;
}

//-----------------------------------------------------------------------------
i32 PacketRouter::count( DestinationID destination_id )
{

   boost::mutex::scoped_lock guard( table_lock_ );

   i32 subscriber_count = -1;
   DestinationIterator destination_iterator = destination_table_.find( destination_id );

   if ( destination_iterator != destination_table_.end() )
   {
      Destination* destination_ptr = destination_iterator->second;

      subscriber_count = destination_ptr->subscribers().size();
   }

   return subscriber_count;
}

//-----------------------------------------------------------------------------
bool PacketRouter::publish( DestinationID destination_id, const void* data_ptr, ui32 length )
{
   bool publish_success = false;
   Destination* destination_ptr = NULL;

   {
      boost::mutex::scoped_lock guard( table_lock_ );
      DestinationIterator destination_iterator = destination_table_.find( destination_id );

      if ( destination_iterator != destination_table_.end() )
      {
         destination_ptr = destination_iterator->second;
      }
   }

   if ( destination_ptr )
   {
      boost::mutex::scoped_lock guard( destination_ptr->lock() );

      SubscriberIterator subscriber_iterator = destination_ptr->subscribers().begin();
      for (; subscriber_iterator != destination_ptr->subscribers().end(); ++subscriber_iterator )
      {
         PacketSubscriber* subscriber_ptr = *subscriber_iterator;

         if ( allowPublish( destination_id, subscriber_ptr, data_ptr, length ) )
         {
            subscriber_ptr->put( destination_id, data_ptr, length );
         }
      }

      publish_success = true;
   }

   return publish_success;
}


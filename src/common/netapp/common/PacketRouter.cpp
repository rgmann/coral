#include "Log.h"
#include "PacketSubscriber.h"
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
  receiver_ptr_ = NULL;
}

//-----------------------------------------------------------------------------
bool PacketRouter::addSubscriber(
  int               subscriber_id,
  PacketSubscriber* subscriber_ptr
)
{
   bool add_success = false;

   if ( subscriber_ptr != NULL )
   {
      boost::mutex::scoped_lock guard( table_lock_ );

      if ( subscriber_table_.count(subscriber_id) == 0 )
      {
         subscriber_ptr->subscribe( subscriber_id, receiver_ptr_ );

         subscriber_table_.insert( std::make_pair( subscriber_id, subscriber_ptr ) );

         add_success = true;
      }
      else
      {
         log::error(
            "PacketRouter::addSubscriber: "
            "Router already has a subcriber with ID = %d\n", subscriber_id);
      }
   }

   return add_success;
}

//-----------------------------------------------------------------------------
PacketSubscriber* PacketRouter::removeSubscriber( int subscriber_id )
{
   boost::mutex::scoped_lock guard( table_lock_ );

   PacketSubscriber* subscriber_ptr = NULL;

   if ( subscriber_table_.count( subscriber_id ) != 0 )
   {
      subscriber_ptr = subscriber_table_.find( subscriber_id )->second;
      subscriber_ptr->unsubscribe();

      subscriber_table_.erase( subscriber_id );
   }
   else
   {
      log::warn("PacketRouter::removeSubscriber: "
             "No subscriber with ID = %d\n", subscriber_id);
   }

   return subscriber_ptr;
}

//-----------------------------------------------------------------------------
ui32 PacketRouter::count()
{
  return subscriber_table_.size();
}

//-----------------------------------------------------------------------------
bool PacketRouter::empty()
{
  return subscriber_table_.empty();
}

//-----------------------------------------------------------------------------
bool PacketRouter::hasSubscriber( int subscriber_id )
{
  boost::mutex::scoped_lock guard( table_lock_ );
  return ( subscriber_table_.count( subscriber_id ) != 0 );
}

//-----------------------------------------------------------------------------
PacketSubscriber* PacketRouter::getSubscriber( int subscriber_id )
{
   boost::mutex::scoped_lock guard( table_lock_ );

   PacketSubscriber* subscriber_ptr = NULL;

   if ( subscriber_table_.count( subscriber_id ) != 0 )
   {
      subscriber_ptr = subscriber_table_.find( subscriber_id )->second;
   }

   return subscriber_ptr;
}

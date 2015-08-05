#include "Log.h"
#include "PacketSubscriber.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
PacketRouter::PacketRouter(PacketReceiver* pReceiver)
: mpReceiver(pReceiver)
{
}

//-----------------------------------------------------------------------------
PacketRouter::~PacketRouter()
{
  mpReceiver = NULL;
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

    if ( mSubscriberTable.count(subscriber_id) == 0 )
    {
      subscriber_ptr->setID( subscriber_id );
      subscriber_ptr->setReceiver( mpReceiver );

      mSubscriberTable.insert( std::make_pair( subscriber_id, subscriber_ptr ) );

      add_success = true;
    }
    else
    {
      log::error("PacketRouter::addSubscriber: "
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

  if ( mSubscriberTable.count( subscriber_id ) != 0 )
  {
    subscriber_ptr = mSubscriberTable.find( subscriber_id )->second;
    mSubscriberTable.erase( subscriber_id );
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
  return mSubscriberTable.size();
}

//-----------------------------------------------------------------------------
bool PacketRouter::empty()
{
  return mSubscriberTable.empty();
}

//-----------------------------------------------------------------------------
bool PacketRouter::hasSubscriber( int subscriber_id )
{
  boost::mutex::scoped_lock guard( table_lock_ );
  return ( mSubscriberTable.count( subscriber_id ) != 0 );
}

//-----------------------------------------------------------------------------
PacketSubscriber* PacketRouter::getSubscriber( int subscriber_id )
{
  boost::mutex::scoped_lock guard( table_lock_ );

  PacketSubscriber* subscriber_ptr = NULL;

  if ( mSubscriberTable.count( subscriber_id ) != 0 )
  {
    subscriber_ptr = mSubscriberTable.find( subscriber_id )->second;
  }

  return subscriber_ptr;
}

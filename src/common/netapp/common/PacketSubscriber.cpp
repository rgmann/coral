#include <string.h>
#include "Log.h"
#include "GenericPacket.h"
#include "PacketReceiver.h"
#include "PacketSubscriber.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
PacketSubscriber::PacketSubscriber()
:  receiver_ptr_( NULL )
,  subscriber_id_( kInvalidSubscriber )
{
}

//-----------------------------------------------------------------------------
PacketSubscriber::~PacketSubscriber()
{
}

//-----------------------------------------------------------------------------
void PacketSubscriber::setID(int id)
{
   subscriber_id_ = id;
}

//-----------------------------------------------------------------------------
void PacketSubscriber::setReceiver(PacketReceiver* pReceiver)
{
   receiver_ptr_ = pReceiver;
}

//-----------------------------------------------------------------------------
void PacketSubscriber::subscribe( int subscriber_id, PacketReceiver* receiver_ptr )
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );
   setID( subscriber_id );
   setReceiver( receiver_ptr );
}

//-----------------------------------------------------------------------------
void PacketSubscriber::unsubscribe()
{
   subscribe( kInvalidSubscriber, NULL );
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::isSubscribed() const
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );
   return ( subscriber_id_ != kInvalidSubscriber );
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::sendPacket(GenericPacket* pPacket)
{
  return sendPacketTo( subscriber_id_, pPacket );
}

//-----------------------------------------------------------------------------
bool PacketSubscriber::sendPacketTo( int destination_id, GenericPacket* packet_ptr )
{
   boost::mutex::scoped_lock guard( subscribe_lock_ );

   bool success = false;

   if ( receiver_ptr_ )
   {
      if ( subscriber_id_ != kInvalidSubscriber )
      {
         success = receiver_ptr_->push( new PacketContainer(
            destination_id, packet_ptr
         ));
      }
      else
      {
         log::error("PacketSubscriber::sendPacketTo - Invalid subscriber ID=%d\n");
      }
   }
   else
   {
      log::error("PacketSubscriber::sendPacketTo - Null receiver\n");
   }

  return success;
}


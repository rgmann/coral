#include <boost/current_function.hpp>
#include "Log.h"
#include "PacketQueue.h"
#include "PacketSubscriber.h"
#include "PacketRouter.h"
#include "NetAppPacket.h"
#include "PacketRelay.h"
#include "gtest/gtest.h"

using namespace coral;
using namespace coral::netapp;

static const DestinationID kFirstChannel  = 5;
static const DestinationID kSecondChannel = 10;
static const DestinationID RELAY_PIPE = 13;

class TestRelayReceiverHook : public RelayReceiverHook {
public:

   TestRelayReceiverHook()
   {
   }

protected:

   PacketContainer* translate( DestinationID destination_id, PacketContainer* in_container_ptr )
   {
      // log::status("TestRelayReceiverHook::translate: %d\n", in_container_ptr->destination_id_);
      NetAppPacket* packet_ptr = new NetAppPacket(
         in_container_ptr->destination_id_,
         in_container_ptr->packet_ptr_->allocatedSize() );

      memcpy(
         packet_ptr->dataPtr(),
         in_container_ptr->packet_ptr_->basePtr(),
         in_container_ptr->packet_ptr_->allocatedSize() );

      PacketContainer* out_container_ptr = new PacketContainer(
         destination_id,
         packet_ptr );
      // log::status("TestRelayReceiverHook::translate: %d, %d\n",
         // out_container_ptr->destination_id_,
         // packet_ptr->data()->type);

      return out_container_ptr;
   }
};

class TestPacketRelay : public PacketRelay {
public:

   TestPacketRelay() : PacketRelay( relay_hook_ ) {}

protected:

   RelayInfo* extract( const RelayInfo& input )
   {
      RelayInfo* output = NULL;

      if ( input.data_ptr && ( input.length >= sizeof(NetAppPacket::Data) ) )
      {
         const NetAppPacket::Data* header_ptr = (const NetAppPacket::Data*)(input.data_ptr);
         const void* data_ptr = &((ui8*)input.data_ptr)[ sizeof(NetAppPacket::Data) ];
         
         output = new RelayInfo(
            header_ptr->type,
            data_ptr,
            header_ptr->length - sizeof(NetAppPacket::Data) );
      }

      return output;
   }


private:

   TestRelayReceiverHook relay_hook_;
};

class TestPacketSubscriber : public PacketSubscriber {
public:

   TestPacketSubscriber()
   : first_channel_count_(0)
   , second_channel_count_(0)
   , unknown_channel_count_(0) {}

   bool put( DestinationID destination_id, const void* data_ptr, ui32 length )
   {
      bool success = true;

      switch ( destination_id )
      {
         case kFirstChannel:
            first_channel_count_++;
            break;

         case kSecondChannel:
            second_channel_count_++;
            break;

         default:
            unknown_channel_count_++;
            success = false;
            break;
      }

      return success;
   }

   bool sendPacket( DestinationID destination_id )
   {
      NetAppPacket* packet_ptr = new NetAppPacket( 0, 100 );
      bool success = sendTo( destination_id, packet_ptr );
      if ( success == false )
      {
         delete packet_ptr;
      }
      return success;
   }

   ui32 first_channel_count_;
   ui32 second_channel_count_;
   ui32 unknown_channel_count_;
};

class RelayTest : public ::testing::Test {
public:

   RelayTest()
   {
   }

protected:

   void SetUp()
   {
      coral::log::level( coral::log::Warn );
   }

   void TearDown()
   {
      coral::log::flush();
   }
};


TEST_F( RelayTest, PublishPacket ) {

   PacketQueue queue;
   PacketRouter router(&queue);
   TestPacketSubscriber subscriber_1;
   TestPacketSubscriber subscriber_2;
   TestPacketRelay relay;

   EXPECT_EQ( -1, router.count( kFirstChannel ) );
   EXPECT_EQ( -1, router.count( kSecondChannel ) );

   // Test registeration
   EXPECT_FALSE( subscriber_1.isSubscribed() );
   EXPECT_FALSE( subscriber_2.isSubscribed() );

   EXPECT_EQ( true, router.subscribe( RELAY_PIPE, &relay, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, relay.subscribe( kFirstChannel, &subscriber_1, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, relay.subscribe( kSecondChannel, &subscriber_2, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, subscriber_1.isSubscribed() );
   EXPECT_EQ( true, subscriber_2.isSubscribed() );
   EXPECT_EQ( 1, relay.count( kFirstChannel ) );
   EXPECT_EQ( 1, relay.count( kSecondChannel ) );

   // Route some packets.
   struct {
      NetAppPacket::Data header;
      char data[256];
   } packet;
   packet.header.type = kFirstChannel;
   packet.header.length = 256;

   EXPECT_EQ( true, router.publish( RELAY_PIPE, &packet, sizeof(packet) ) );
   EXPECT_EQ( 1, subscriber_1.first_channel_count_ );
   EXPECT_EQ( 0, subscriber_1.second_channel_count_ );
   EXPECT_EQ( 0, subscriber_2.first_channel_count_ );
   EXPECT_EQ( 0, subscriber_2.second_channel_count_ );

   packet.header.type = kSecondChannel;
   EXPECT_EQ( true, router.publish( RELAY_PIPE, &packet, sizeof(packet) ) );
   EXPECT_EQ( 1, subscriber_1.first_channel_count_ );
   EXPECT_EQ( 0, subscriber_1.second_channel_count_ );
   EXPECT_EQ( 0, subscriber_2.first_channel_count_ );
   EXPECT_EQ( 1, subscriber_2.second_channel_count_ );

   // Test unregisteration
   EXPECT_EQ( true, relay.unsubscribe( kFirstChannel, &subscriber_1 ) );
   EXPECT_EQ( true, relay.unsubscribe( kSecondChannel, &subscriber_2 ) );
   EXPECT_EQ( true, router.unsubscribe( RELAY_PIPE, &relay ) );
   EXPECT_FALSE( subscriber_1.isSubscribed() );
   EXPECT_FALSE( subscriber_2.isSubscribed() );
   EXPECT_EQ( 0, relay.count( kFirstChannel ) );
   EXPECT_EQ( 0, relay.count( kSecondChannel ) );
   EXPECT_EQ( 0, router.count( RELAY_PIPE ) );
}

TEST_F( RelayTest, CollectPacket ) {

   PacketQueue queue;
   PacketRouter router(&queue);
   TestPacketSubscriber subscriber_1;
   TestPacketSubscriber subscriber_2;
   TestPacketRelay relay;

   EXPECT_EQ( -1, router.count( kFirstChannel ) );
   EXPECT_EQ( -1, router.count( kSecondChannel ) );

   // Test registeration
   EXPECT_FALSE( subscriber_1.isSubscribed() );
   EXPECT_FALSE( subscriber_2.isSubscribed() );

   EXPECT_EQ( true, router.subscribe( RELAY_PIPE, &relay, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, relay.subscribe( kFirstChannel, &subscriber_1, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, relay.subscribe( kSecondChannel, &subscriber_2, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, subscriber_1.isSubscribed() );
   EXPECT_EQ( true, subscriber_2.isSubscribed() );
   EXPECT_EQ( 1, relay.count( kFirstChannel ) );
   EXPECT_EQ( 1, relay.count( kSecondChannel ) );

   // Route some packets.
   EXPECT_EQ( true, subscriber_1.sendPacket( kFirstChannel ) );
   EXPECT_EQ( true, subscriber_2.sendPacket( kSecondChannel ) );

   PacketContainer* container_ptr = queue.pop();
   EXPECT_TRUE( ( container_ptr != NULL ) );
   EXPECT_EQ( RELAY_PIPE, container_ptr->destination_id_ );
   NetAppPacket::Data* header_ptr = (NetAppPacket::Data*)container_ptr->packet_ptr_->basePtr();
   EXPECT_EQ( kFirstChannel, (DestinationID)header_ptr->type );

   container_ptr = queue.pop();
   EXPECT_TRUE( ( container_ptr != NULL ) );
   EXPECT_EQ( RELAY_PIPE, container_ptr->destination_id_ );
   header_ptr = (NetAppPacket::Data*)container_ptr->packet_ptr_->basePtr();
   EXPECT_EQ( kSecondChannel, (DestinationID)header_ptr->type );

   // Test unregisteration
   EXPECT_EQ( true, relay.unsubscribe( kFirstChannel, &subscriber_1 ) );
   EXPECT_EQ( true, relay.unsubscribe( kSecondChannel, &subscriber_2 ) );
   EXPECT_EQ( true, router.unsubscribe( RELAY_PIPE, &relay ) );
   EXPECT_FALSE( subscriber_1.isSubscribed() );
   EXPECT_FALSE( subscriber_2.isSubscribed() );
   EXPECT_EQ( 0, relay.count( kFirstChannel ) );
   EXPECT_EQ( 0, relay.count( kSecondChannel ) );
   EXPECT_EQ( 0, router.count( RELAY_PIPE ) );
}


#include <boost/current_function.hpp>
#include "Log.h"
#include "PacketQueue.h"
#include "PacketSubscriber.h"
#include "PacketRouter.h"
#include "NetAppPacket.h"
#include "gtest/gtest.h"

using namespace coral;
using namespace coral::netapp;


class PacketTest : public ::testing::Test {
public:

   PacketTest()
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


TEST_F( PacketTest, RegisterUnregister ) {
   class TestPacketSubscriber : public PacketSubscriber {
   public:

      bool put( DestinationID destination_id, const void* data_ptr, ui32 length )
      {
         return true;
      }
   };

   PacketQueue queue;
   PacketRouter router(&queue);
   TestPacketSubscriber subscriber;

   const DestinationID kTestChannel = 5;

   EXPECT_EQ( -1, router.count( kTestChannel ) );

   // Test registeration
   EXPECT_FALSE( subscriber.isSubscribed() );
   EXPECT_EQ( true, router.subscribe( kTestChannel, &subscriber, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, subscriber.isSubscribed() );
   EXPECT_EQ( 1, router.count( kTestChannel ) );

   // Test unregisteration
   EXPECT_EQ( true, router.unsubscribe( kTestChannel, &subscriber ) );
   EXPECT_FALSE( subscriber.isSubscribed() );
   EXPECT_EQ( 0, router.count( kTestChannel ) );
}

TEST_F( PacketTest, ReceivePacket ) {
   const DestinationID kFirstChannel  = 5;
   const DestinationID kSecondChannel = 10;

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

      ui32 first_channel_count_;
      ui32 second_channel_count_;
      ui32 unknown_channel_count_;
   };

   PacketQueue queue;
   PacketRouter router(&queue);
   TestPacketSubscriber subscriber_1;
   TestPacketSubscriber subscriber_2;

   EXPECT_EQ( -1, router.count( kFirstChannel ) );
   EXPECT_EQ( -1, router.count( kSecondChannel ) );

   // Test registeration
   EXPECT_FALSE( subscriber_1.isSubscribed() );
   EXPECT_FALSE( subscriber_2.isSubscribed() );

   EXPECT_EQ( true, router.subscribe( kFirstChannel, &subscriber_1, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, router.subscribe( kFirstChannel, &subscriber_2, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, router.subscribe( kSecondChannel, &subscriber_2, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, subscriber_1.isSubscribed() );
   EXPECT_EQ( true, subscriber_2.isSubscribed() );
   EXPECT_EQ( 2, router.count( kFirstChannel ) );
   EXPECT_EQ( 1, router.count( kSecondChannel ) );

   // Route some packets.
   char* data_ptr = new char[ 30 ];
   EXPECT_EQ( true, router.publish( kFirstChannel, data_ptr, 30 ) );
   EXPECT_EQ( true, router.publish( kSecondChannel, data_ptr, 30 ) );
   EXPECT_EQ( 1, subscriber_1.first_channel_count_ );
   EXPECT_EQ( 0, subscriber_1.second_channel_count_ );
   EXPECT_EQ( 1, subscriber_2.first_channel_count_ );
   EXPECT_EQ( 1, subscriber_2.second_channel_count_ );

   // Test unregisteration
   EXPECT_EQ( true, router.unsubscribe( kFirstChannel, &subscriber_1 ) );
   EXPECT_EQ( true, router.unsubscribe( kFirstChannel, &subscriber_2 ) );
   EXPECT_EQ( true, router.unsubscribe( kSecondChannel, &subscriber_2 ) );
   EXPECT_FALSE( subscriber_1.isSubscribed() );
   EXPECT_FALSE( subscriber_2.isSubscribed() );
   EXPECT_EQ( 0, router.count( kFirstChannel ) );
   EXPECT_EQ( 0, router.count( kSecondChannel ) );
}

TEST_F( PacketTest, SendPacket ) {
   const DestinationID kFirstChannel  = 5;
   const DestinationID kSecondChannel = 10;

   class TestPacketSubscriber : public PacketSubscriber {
   public:

      bool sendPacket( DestinationID destination_id )
      {
         NetAppPacket* packet_ptr = new NetAppPacket( destination_id, 100 );
         bool success = sendTo( destination_id, packet_ptr );
         if ( success == false )
         {
            delete packet_ptr;
         }
         return success;
      }

      bool put( DestinationID destination_id, const void* data_ptr, ui32 length )
      {
         return true;
      }
   };

   PacketQueue queue;
   PacketRouter router(&queue);
   TestPacketSubscriber subscriber;

   EXPECT_EQ( -1, router.count( kFirstChannel ) );
   EXPECT_EQ( -1, router.count( kSecondChannel ) );

   // Test registeration
   EXPECT_FALSE( subscriber.isSubscribed() );
   EXPECT_EQ( true, router.subscribe( kFirstChannel, &subscriber, kSubscriberModeReadWrite ) );
   EXPECT_EQ( true, router.subscribe( kSecondChannel, &subscriber, kSubscriberModeReadOnly ) );
   EXPECT_EQ( true, subscriber.isSubscribed() );
   EXPECT_EQ( 1, router.count( kFirstChannel ) );
   EXPECT_EQ( 1, router.count( kSecondChannel ) );

   // Send a packet on the first channel.
   EXPECT_EQ( true, subscriber.sendPacket( kFirstChannel ) );
   PacketContainer* container_ptr = queue.pop();
   EXPECT_TRUE( ( container_ptr != NULL ) );
   EXPECT_EQ( kFirstChannel, container_ptr->destination_id_ );
   EXPECT_EQ( 108, container_ptr->packet_ptr_->allocatedSize() );
   // delete container_ptr->packet_ptr_;
   // delete container_ptr;


   // Test unregisteration
   EXPECT_EQ( true, router.unsubscribe( kFirstChannel, &subscriber ) );
   EXPECT_EQ( true, router.unsubscribe( kSecondChannel, &subscriber ) );
   EXPECT_FALSE( subscriber.isSubscribed() );

   EXPECT_EQ( 0, router.count( kFirstChannel ) );
   EXPECT_EQ( 0, router.count( kSecondChannel ) );
}


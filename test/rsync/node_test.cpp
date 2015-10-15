#ifndef  NODE_TEST_CPP
#define  NODE_TEST_CPP

#include <boost/current_function.hpp>
#include "Log.h"
#include "RsyncNode.h"
#include "IntraRouter.h"
#include "gtest/gtest.h"

using namespace coral;
using namespace coral::rsync;

class NodeTest : public ::testing::Test {
public:

   NodeTest()
   : workgroup_(NULL)
   , node_( NULL )
   {
     LOCAL_ROOT = boost::filesystem::current_path() / "test_root/client";
     REMOTE_ROOT = boost::filesystem::current_path() / "test_root/server";
   }

   boost::filesystem::path LOCAL_ROOT;
   boost::filesystem::path REMOTE_ROOT;


protected:

   void SetUp()
   {
      coral::log::level( coral::log::Warn );

      workgroup_ = new WorkerGroup();
      node_ = new RsyncNode( LOCAL_ROOT, *workgroup_ );
      router_.launch();

      ASSERT_EQ( true, router_.subscribe( RSYNC_SUB_ID, &node_->subscriber()) );

      // node_->setCallback( &mLocalCallback );
   }

   void TearDown()
   {
      router_.cancel(true);
      router_.unsubscribe( RSYNC_SUB_ID, &node_->subscriber() );


      node_->unsetJobCompletionCallback();

      delete node_;
      delete workgroup_;
   }


   IntraRouter router_;
   WorkerGroup* workgroup_;
   RsyncNode* node_;

};


TEST_F( NodeTest, InvalidLocalSourcePath ) {
   boost::filesystem::path source( "invalid_dir/s_inst.cpp" );
   boost::filesystem::path destination( "d_inst.cpp" );

   EXPECT_EQ( kRsyncSourceFileNotFound, node_->sync(
      LocalResourcePath( destination ),
      LocalResourcePath( source ) ) );
}

TEST_F( NodeTest, InvalidLocalDestinationPath ) {
   boost::filesystem::path source( "s_inst.cpp" );
   boost::filesystem::path destination( "invalid_dir/d_inst.cpp" );

   EXPECT_EQ( kRsyncDestinationFileNotFound, node_->sync(
      LocalResourcePath( destination ),
      LocalResourcePath( source ) ) );
}

TEST_F( NodeTest, RemoteAuthRequestTimeout ) {
   boost::filesystem::path source( "s_inst.cpp" );
   boost::filesystem::path destination( "d_inst.cpp" );

   class TestCallback : public RsyncJobCallback {
   public:

      void call(const JobDescriptor& job, const JobReport& report)
      {
         coral::log::status("%s\n", BOOST_CURRENT_FUNCTION);
         error_ = report.source.authority.status;
         sem_.give();
      }

      BinarySem sem_;
      RsyncError error_;
   } callback;

   node_->setJobCompletionCallback( &callback );
   node_->setCompletionTimeout(2000);

   EXPECT_EQ( kRsyncSuccess, node_->sync(
      LocalResourcePath( destination ),
      RemoteResourcePath( source ) ) );

   callback.sem_.take();
   EXPECT_EQ( kRsyncRemoteQueryTimeout, callback.error_ );
}


TEST_F( NodeTest, RemoteSourceDoesNotExist ) {
   IntraRouter remote_router;

   boost::filesystem::path source( "non_existent.cpp" );
   boost::filesystem::path destination( "d_inst.cpp" );

   class TestCallback : public RsyncJobCallback {
   public:

      void call( const JobDescriptor& job, const JobReport& report )
      {
         error_ = report.source.authority.status;
         sem_.give();
      }

      BinarySem sem_;
      RsyncError error_;
   } callback;

   node_->setJobCompletionCallback( &callback );

   router_.setCounterpart( &remote_router );
   remote_router.setCounterpart( &router_ );

   WorkerGroup remote_workgroup;
   RsyncNode remote_node( REMOTE_ROOT, remote_workgroup );
   remote_router.launch();
   ASSERT_EQ( true, remote_router.subscribe( RSYNC_SUB_ID, &remote_node.subscriber()) );

   EXPECT_EQ( kRsyncSuccess, node_->sync(
      LocalResourcePath( destination ),
      RemoteResourcePath( source ) ) );

   callback.sem_.take();

   EXPECT_EQ( kRsyncSourceFileNotFound, callback.error_ );

   remote_router.cancel();
   remote_router.unsubscribe( RSYNC_SUB_ID, &remote_node.subscriber() );

   router_.setCounterpart( NULL );
}

#endif  // NODE_TEST_CPP

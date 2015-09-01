#include <fstream>
#include <iostream>
#include "Log.h"
#include "RsyncNode.h"
#include "IntraRouter.h"
#include "gtest/gtest.h"


#define  RSYNC_SUB_ID  ( 1 )

using namespace liber::netapp;
using namespace liber::rsync;


class TestCallback : public RsyncJobCallback {
public:

  TestCallback(const char* pName) : mSem(0), mpName(pName) {};

  void call(const JobDescriptor& job, const JobReport& report)
  {
    liber::log::status("%s: Completed %s\n", mpName, job.getDestination().path().string().c_str());
    // liber::log::flush();

    // report.print(std::cout);
    // std::cout << "\n\n";

    mSem.give();
  }

  CountingSem mSem;
  const char* mpName;
};

// namespace RsyncTestSuite {
  class EndToEndTest : public ::testing::Test {
  public:

    EndToEndTest()
    : mLocalWorkGroup(NULL)
    , mLocalNode( NULL )
    , mRemoteWorkGroup( NULL )
    , mRemoteNode( NULL )
    , mLocalCallback( "Local" )
    , mRemoteCallback( "Remote" )
    {
      LOCAL_ROOT = boost::filesystem::current_path() / "test_root/client";
      REMOTE_ROOT = boost::filesystem::current_path() / "test_root/server";
    }

    boost::filesystem::path LOCAL_ROOT;
    boost::filesystem::path REMOTE_ROOT;

  protected:

    void GetFilesListing(
      const char* pPath,
      std::vector<boost::filesystem::path>& listing,
      bool bRecursive = false )
    {
      boost::filesystem::path directory( pPath );
      boost::filesystem::directory_iterator endIter;

      // std::cout << "PATH: " << pPath << std::endl;
      if ( boost::filesystem::exists( directory ) &&
           boost::filesystem::is_directory( directory ) )
      {
        for( boost::filesystem::directory_iterator dirIter( directory ) ; dirIter != endIter ; ++dirIter)
        {
          if ( boost::filesystem::is_regular_file( dirIter->status() ) )
          {
            // std::cout << dirIter->path().string() << std::endl;
            listing.push_back( dirIter->path() );
          }
          else if ( bRecursive )
          {
            GetFilesListing( dirIter->path().c_str(), listing, bRecursive );
          }
        }
      }
    }

    void DeleteContents( const char* pPath )
    {
      std::vector<boost::filesystem::path> listing;

      GetFilesListing( pPath, listing, true );

      std::vector<boost::filesystem::path>::iterator file = listing.begin();
      for (; file != listing.end(); file++ )
      {
        if ( boost::filesystem::exists( *file ) )
        {
          boost::filesystem::remove( *file );
        }
      }
    }

    void CopyContents( const char* pSrcPath, const char* pDstPath )
    {
      boost::system::error_code error;
      std::vector<boost::filesystem::path> listing;

      GetFilesListing( pSrcPath, listing );

      std::vector<boost::filesystem::path>::iterator file = listing.begin();
      for (; file != listing.end(); file++ )
      {
        if ( boost::filesystem::exists( *file ) )
        {
          boost::filesystem::copy_file(
            *file,
            boost::filesystem::path( pDstPath ) / file->filename(),
            boost::filesystem::copy_option::none,
            error );

          if ( error != boost::system::errc::success )
            std::cout << "Failed to copy " << file->string()
                      << " to " << boost::filesystem::path( pDstPath ) << std::endl;
        }
      }
    }

    bool CheckEqual(
      const boost::filesystem::path& rSource,
      const boost::filesystem::path& rDestination )
    {
      bool bEqual = true;

      std::ifstream sourceStream(
        rSource.string().c_str(), std::ifstream::in | std::ifstream::binary );
      std::ifstream destinationStream(
        rDestination.string().c_str(), std::ifstream::in | std::ifstream::binary );

      if ( sourceStream.is_open() && destinationStream.is_open() )
      {
        char sourceChar = 0;
        char destinationChar = 0;
        int  nByteIndex = 0;

        while ( bEqual )
        {
          sourceStream.get( sourceChar );
          destinationStream.get( destinationChar );

          if ( sourceStream.eof() && destinationStream.eof() )
          {
            // Successful completion.
            break;
          }
          else if ( !sourceStream.good() || !destinationStream.good() )
          {
            // One file reached EOF while the other did not.
            bEqual = false;
          }
          else
          {
            if ( sourceChar != destinationChar )
            {
              bEqual = false;
            }
          }

          if ( !bEqual )
          {
            std::cout << "Mismatch between " << rSource
                      << " and " << rDestination
                      << " at " << nByteIndex << std::endl;
          }

          nByteIndex++;
        }
      }
      else
      {
        bEqual = false;
        std::cout << "Failed to open " << rSource << " or " << rDestination << std::endl;
      }

      sourceStream.close();
      destinationStream.close();

      return bEqual;
    }

    void SetUp()
    {
      liber::log::level( liber::log::Warn );

      DeleteContents( "./test_root/client" );
      DeleteContents( "./test_root/server" );

      CopyContents( "./test_root/originals/client", "./test_root/client" );
      CopyContents( "./test_root/originals/server", "./test_root/server" );

      mLocalWorkGroup = new WorkerGroup();
      mRemoteWorkGroup = new WorkerGroup();

      mLocalNode = new RsyncNode( LOCAL_ROOT, *mLocalWorkGroup );
      mRemoteNode = new RsyncNode( REMOTE_ROOT, *mRemoteWorkGroup );

      mLocalRouter.setCounterpart(&mRemoteRouter);
      mRemoteRouter.setCounterpart(&mLocalRouter);

      mLocalRouter.launch();
      mRemoteRouter.launch();

      ASSERT_EQ( true, mLocalRouter.subscribe(RSYNC_SUB_ID, &mLocalNode->subscriber() ) );
      ASSERT_EQ( true, mRemoteRouter.subscribe(RSYNC_SUB_ID, &mRemoteNode->subscriber()) );

      mLocalNode->setJobCompletionCallback( &mLocalCallback );
      mRemoteNode->setJobCompletionCallback( &mRemoteCallback );
    }

    void TearDown()
    {
      mLocalRouter.cancel(true);
      mRemoteRouter.cancel(true);

      mLocalRouter.unsubscribe( RSYNC_SUB_ID, &mLocalNode->subscriber() );
      mRemoteRouter.unsubscribe( RSYNC_SUB_ID, &mRemoteNode->subscriber() );

      mLocalNode->unsetJobCompletionCallback();
      mRemoteNode->unsetJobCompletionCallback();

      delete mRemoteNode;
      delete mLocalNode;

      delete mRemoteWorkGroup;
      delete mLocalWorkGroup;
    }

    IntraRouter mLocalRouter;
    WorkerGroup* mLocalWorkGroup;
    RsyncNode* mLocalNode;

    IntraRouter mRemoteRouter;
    WorkerGroup* mRemoteWorkGroup;
    RsyncNode*  mRemoteNode;

    TestCallback mLocalCallback;
    TestCallback mRemoteCallback;
  };
// }

//
// Synchronize two files where the source and destination are local.
//
TEST_F( EndToEndTest, LocalToLocal ) {
  boost::filesystem::path source( "s_inst.cpp" );
  boost::filesystem::path destination( "d_inst.cpp" );;

  RsyncError sync_status = mLocalNode->sync(
    LocalResourcePath( destination ),
    LocalResourcePath( source ) );
  EXPECT_EQ( kRsyncSuccess, sync_status );
  if ( sync_status == kRsyncSuccess )
  {
    EXPECT_EQ( true, mLocalCallback.mSem.take() );

    EXPECT_EQ( true, CheckEqual(
      boost::filesystem::path( LOCAL_ROOT ) / destination,
      boost::filesystem::path( LOCAL_ROOT ) / source
    ));
  }
}

//
// Synchronize two files where the destination is remote and the source is
// local. Note: Source file is smaller than destination.
//
TEST_F( EndToEndTest, RemoteDestSmallTest ) {
  boost::filesystem::path source( "file.dat" );
  boost::filesystem::path destination( "file.dat" );

  RsyncError sync_status = mLocalNode->sync(
    RemoteResourcePath( destination ),
    LocalResourcePath( source ) );
  EXPECT_EQ( kRsyncSuccess, sync_status );
  if ( sync_status == kRsyncSuccess )
  {
    EXPECT_EQ( true, mLocalCallback.mSem.take() );
    EXPECT_EQ( true, mRemoteCallback.mSem.take() );

    EXPECT_EQ( true, CheckEqual(
      boost::filesystem::path( REMOTE_ROOT ) / destination,
      boost::filesystem::path( LOCAL_ROOT ) / source ) );
  }
}

//
// Synchronize two images where the destination is remote and the source is
// local.
//
TEST_F( EndToEndTest, RemoteDestImageTest ) {
  boost::filesystem::path source( "image.png" );
  boost::filesystem::path destination( "image.png" );

  RsyncError sync_status = mLocalNode->sync(
    RemoteResourcePath( destination ),
    LocalResourcePath( source ) );
  EXPECT_EQ( kRsyncSuccess, sync_status );
  if ( sync_status == kRsyncSuccess )
  {
    EXPECT_EQ( true, mLocalCallback.mSem.take() );
    EXPECT_EQ( true, mRemoteCallback.mSem.take() );

    EXPECT_EQ( true, CheckEqual(
      boost::filesystem::path( REMOTE_ROOT ) / destination,
      boost::filesystem::path( LOCAL_ROOT ) / source ) );
  }
}

//
// Synchronize two files where the source is remote and the destination is
// local.
//
TEST_F( EndToEndTest, RemoteSourceTest ) {
  boost::filesystem::path source( "instruction_test.cpp" );
  boost::filesystem::path destination( "instruction_test.cpp" );

  RsyncError sync_status = mLocalNode->sync(
    LocalResourcePath( destination ),
    RemoteResourcePath( source) );

  EXPECT_EQ( kRsyncSuccess, sync_status );
  if ( sync_status == kRsyncSuccess )
  {
    EXPECT_EQ( true, mLocalCallback.mSem.take() );
    EXPECT_EQ( true, mRemoteCallback.mSem.take() );

    EXPECT_EQ( true, CheckEqual(
      boost::filesystem::path( REMOTE_ROOT ) / destination,
      boost::filesystem::path( LOCAL_ROOT ) / source ) );
  }
}

//
// Test creating a local file from a remote source when local version does
// not exist.
//
TEST_F( EndToEndTest, RemoteSourceCopy ) {
  boost::filesystem::path source( "dfile_0.dat" );
  boost::filesystem::path destination( "dfile_0.dat" );

  RsyncError sync_status = mLocalNode->sync(
    LocalResourcePath( destination ),
    RemoteResourcePath( source ) );

  EXPECT_EQ( kRsyncSuccess, sync_status );
  if ( sync_status == kRsyncSuccess )
  {
    EXPECT_EQ( true, mLocalCallback.mSem.take() );

    EXPECT_EQ( true, CheckEqual(
      boost::filesystem::path( REMOTE_ROOT ) / destination,
      boost::filesystem::path( LOCAL_ROOT ) / source ) );
  }
}

typedef struct op_attrs {
  boost::filesystem::path path;
  RsyncError status;
} op_attrs_t;
TEST_F( EndToEndTest, PipelineTest ) {

  std::vector<op_attrs_t> operations;

  op_attrs_t path;
  path.status = kRsyncSuccess;

  path.path = "instruction_test.cpp";
  operations.push_back( path );
  path.path = "dfile_0.dat";
  operations.push_back( path );
  path.path = "screenshot.png";
  operations.push_back( path );
  
  std::vector<op_attrs_t>::iterator pathIt = operations.begin();
  for (; pathIt != operations.end(); pathIt++ )
  {
    pathIt->status = mLocalNode->sync(
      LocalResourcePath( pathIt->path ),
      RemoteResourcePath( pathIt->path ) );

    EXPECT_EQ( kRsyncSuccess, pathIt->status );
  }

  pathIt = operations.begin();
  for (; pathIt != operations.end(); pathIt++ )
  {
    std::cout << "Waiting for " << pathIt->path << std::endl;
    if ( pathIt->status == kRsyncSuccess ) EXPECT_EQ( true, mLocalCallback.mSem.take() );
    std::cout << "Checking " << pathIt->path << std::endl;
    EXPECT_EQ( true, CheckEqual(
      boost::filesystem::path( REMOTE_ROOT ) / pathIt->path,
      boost::filesystem::path( LOCAL_ROOT ) / pathIt->path ) );

    std::cout << "Done checking " << pathIt->path << std::endl;
  }
}

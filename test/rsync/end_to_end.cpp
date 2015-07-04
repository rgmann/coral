#include <fstream>
#include <iostream>
#include "Log.h"
#include "RsyncNode.h"
#include "IntraRouter.h"
#include "gtest/gtest.h"


#define  LOCAL_ROOT    ((const char*)"/Users/vaughanbiker/Development/liber/test/rsync/test_root/client")
#define  REMOTE_ROOT   ((const char*)"/Users/vaughanbiker/Development/liber/test/rsync/test_root/server")
#define  RSYNC_SUB_ID  ( 1 )

using namespace liber::netapp;
using namespace liber::rsync;

class TestCallback : public RsyncJobCallback {
public:

  TestCallback(const char* pName) : mSem(0), mpName(pName) {};

  void call(const JobDescriptor& job, const JobReport& report)
  {
    liber::log::status("%s: Completed %s\n", mpName, job.getDestination().path.string().c_str());
    liber::log::flush();

    report.print(std::cout);
    std::cout << "\n\n";

    mSem.give();
  }

  CountingSem mSem;
  const char* mpName;
};

// namespace RsyncTestSuite {
  class EndToEndTest : public ::testing::Test {
  public:

    EndToEndTest()
    : mLocalNode( NULL )
    , mRemoteNode( NULL )
    , mLocalCallback( "Local" )
    , mRemoteCallback( "Remote" )
    {
    }

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
        rSource.string(), std::ifstream::in | std::ifstream::binary );
      std::ifstream destinationStream(
        rDestination.string(), std::ifstream::in | std::ifstream::binary );

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
      DeleteContents( "./test_root/client" );
      DeleteContents( "./test_root/server" );

      CopyContents( "./test_root/originals/client", "./test_root/client" );
      CopyContents( "./test_root/originals/server", "./test_root/server" );

      mLocalNode = new RsyncNode( LOCAL_ROOT );
      mRemoteNode = new RsyncNode( REMOTE_ROOT );

      mLocalRouter.setCounterpart(&mRemoteRouter);
      mRemoteRouter.setCounterpart(&mLocalRouter);

      mLocalRouter.launch();
      mRemoteRouter.launch();

      ASSERT_EQ( true, mLocalRouter.addSubscriber(RSYNC_SUB_ID, &mLocalNode->subscriber()) );
      ASSERT_EQ( true, mRemoteRouter.addSubscriber(RSYNC_SUB_ID, &mRemoteNode->subscriber()) );

      mLocalNode->setCallback( &mLocalCallback );
      mRemoteNode->setCallback( &mRemoteCallback );
    }

    void TearDown()
    {
      liber::log::status("Cancel threads\n");
      mLocalRouter.cancel(true);
      mRemoteRouter.cancel(true);

      liber::log::status("Remove subscribers\n");
      mLocalRouter.removeSubscriber( RSYNC_SUB_ID );
      mRemoteRouter.removeSubscriber( RSYNC_SUB_ID );

      liber::log::status("Unset callbacks\n");
      mLocalNode->unsetCallback();
      mRemoteNode->unsetCallback();

      liber::log::status("DELETE REMOTE\n");
      delete mRemoteNode;
      liber::log::status("DELETE LOCAL\n");
      delete mLocalNode;

      liber::log::status("flush\n");
      liber::log::flush();
      liber::log::status("DONE\n");
    }

    IntraRouter mLocalRouter;
    RsyncNode* mLocalNode;

    IntraRouter mRemoteRouter;
    RsyncNode* mRemoteNode;

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

  EXPECT_EQ( RsyncSuccess, mLocalNode->sync( destination, source ) );
  EXPECT_EQ( true, mLocalCallback.mSem.take() );

  EXPECT_EQ( true, CheckEqual(
    boost::filesystem::path( LOCAL_ROOT ) / destination,
    boost::filesystem::path( LOCAL_ROOT ) / source
  ));
}

//
// Synchronize two files where the destination is remote and the source is
// local. Note: Source file is smaller than destination.
//
TEST_F( EndToEndTest, RemoteDestSmallTest ) {
  boost::filesystem::path source( "file.dat" );
  boost::filesystem::path destination( "file.dat" );

  EXPECT_EQ( RsyncSuccess, mLocalNode->sync(destination, source, true, false) );
  EXPECT_EQ( true, mLocalCallback.mSem.take() );
  EXPECT_EQ( true, mRemoteCallback.mSem.take() );

  EXPECT_EQ( true, CheckEqual(
    boost::filesystem::path( REMOTE_ROOT ) / destination,
    boost::filesystem::path( LOCAL_ROOT ) / source ) );
}

//
// Synchronize two images where the destination is remote and the source is
// local.
//
TEST_F( EndToEndTest, RemoteDestImageTest ) {
  boost::filesystem::path source( "image.png" );
  boost::filesystem::path destination( "image.png" );

  EXPECT_EQ( RsyncSuccess, mLocalNode->sync(destination, source, true, false) );
  EXPECT_EQ( true, mLocalCallback.mSem.take() );
  EXPECT_EQ( true, mRemoteCallback.mSem.take() );

  EXPECT_EQ( true, CheckEqual(
    boost::filesystem::path( REMOTE_ROOT ) / destination,
    boost::filesystem::path( LOCAL_ROOT ) / source ) );
}

//
// Synchronize two files where the source is remote and the destination is
// local.
//
TEST_F( EndToEndTest, RemoteSourceTest ) {
  boost::filesystem::path source( "instruction_test.cpp" );
  boost::filesystem::path destination( "instruction_test.cpp" );

  EXPECT_EQ( RsyncSuccess, mLocalNode->sync(destination, source, false, true ) );
  EXPECT_EQ( true, mLocalCallback.mSem.take() );

  EXPECT_EQ( true, CheckEqual(
    boost::filesystem::path( REMOTE_ROOT ) / destination,
    boost::filesystem::path( LOCAL_ROOT ) / source ) );
}

//
// Test creating a local file from a remote source when local version does
// not exist.
//
TEST_F( EndToEndTest, RemoteSourceCopy ) {
  boost::filesystem::path source( "dfile_0.dat" );
  boost::filesystem::path destination( "dfile_0.dat" );

  EXPECT_EQ( RsyncSuccess, mLocalNode->sync(destination, source, false, true ) );
  EXPECT_EQ( true, mLocalCallback.mSem.take() );

  EXPECT_EQ( true, CheckEqual(
    boost::filesystem::path( REMOTE_ROOT ) / destination,
    boost::filesystem::path( LOCAL_ROOT ) / source ) );
}

typedef struct op_attrs {
  boost::filesystem::path path;
  RsyncError status;
} op_attrs_t;
TEST_F( EndToEndTest, PipelineTest ) {

  std::vector<op_attrs_t> operations;

  op_attrs_t path;
  path.status = RsyncSuccess;

  // path.path = "instruction_test.cpp";
  // operations.push_back( path );
  // path.path = "dfile_0.dat";
  // operations.push_back( path );
  path.path = "screenshot.png";
  operations.push_back( path );
  
  std::vector<op_attrs_t>::iterator pathIt = operations.begin();
  for (; pathIt != operations.end(); pathIt++ )
  {
    pathIt->status = mLocalNode->sync( pathIt->path, pathIt->path, false, true );
    EXPECT_EQ( RsyncSuccess, pathIt->status );
  }

  pathIt = operations.begin();
  for (; pathIt != operations.end(); pathIt++ )
  {
    std::cout << "Waiting for " << pathIt->path << std::endl;
    if ( pathIt->status == RsyncSuccess ) EXPECT_EQ( true, mLocalCallback.mSem.take() );
    std::cout << "Checking " << pathIt->path << std::endl;
    EXPECT_EQ( true, CheckEqual(
      boost::filesystem::path( REMOTE_ROOT ) / pathIt->path,
      boost::filesystem::path( LOCAL_ROOT ) / pathIt->path ) );

    std::cout << "Done checking " << pathIt->path << std::endl;
  }
}

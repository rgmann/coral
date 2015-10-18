// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 

#ifndef  WORKER_TEST_CPP
#define  WORKER_TEST_CPP

#include <fstream>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include "Log.h"
#include "RsyncNode.h"
#include "IntraRouter.h"
#include "gtest/gtest.h"


#define  RSYNC_SUB_ID  ( 1 )

using namespace coral::netapp;
using namespace coral::rsync;


class WorkerTestCallback : public RsyncJobCallback {
public:

  WorkerTestCallback(const char* pName) : mSem(0), mpName(pName) {};

  void call(const JobDescriptor& job, const JobReport& report)
  {
    // coral::log::status("%s: Completed %s\n", mpName, job.getDestination().path().string().c_str());
    coral::log::status("%s Report:\n%s\n", mpName, report.toString().c_str() );

    mSem.give();
  }

  CountingSem mSem;
  const char* mpName;
};

// namespace RsyncTestSuite {
class WorkerTest : public ::testing::Test {
public:

   WorkerTest()
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
    coral::log::level( coral::log::Verbose );

    DeleteContents( "./test_root/client" );
    DeleteContents( "./test_root/server" );

    CopyContents( "./test_root/originals/client", "./test_root/client" );
    CopyContents( "./test_root/originals/server", "./test_root/server" );
   }

   void TearDown()
   {
      coral::log::flush();
   }
};

TEST_F( WorkerTest, WorkGroupSetupTeardown )
{
   for ( size_t workerCount = 1; workerCount <= 3; ++workerCount )
   {
      WorkerGroup workgroup( workerCount );

      boost::this_thread::sleep(boost::posix_time::milliseconds(100));
      EXPECT_EQ( workerCount, workgroup.workers().size() );
   }
}

namespace worker_test {
typedef struct op_attrs {
   boost::filesystem::path path;
   RsyncError status;
} op_attrs_t;
}
TEST_F( WorkerTest, MultiWorkerPipelineTest ) {

   IntraRouter serverRouter;
   IntraRouter clientRouter;

   serverRouter.setCounterpart(&clientRouter);
   clientRouter.setCounterpart(&serverRouter);

   serverRouter.launch();
   clientRouter.launch();

   WorkerGroup serverWorkgroup( 1 );
   WorkerGroup clientWorkgroup;

   WorkerTestCallback serverCallback("SERVER");
   WorkerTestCallback clientCallback("CLIENT");

   RsyncNode serverNode( REMOTE_ROOT, serverWorkgroup );
   RsyncNode clientNode( LOCAL_ROOT, clientWorkgroup );


   ASSERT_EQ( true, clientRouter.subscribe( RSYNC_SUB_ID, &clientNode.subscriber() ) );
   ASSERT_EQ( true, serverRouter.subscribe( RSYNC_SUB_ID, &serverNode.subscriber() ) );

   clientNode.setJobCompletionCallback( &clientCallback );
   serverNode.setJobCompletionCallback( &serverCallback );


   std::vector<worker_test::op_attrs_t> operations;

   worker_test::op_attrs_t path;
   path.status = kRsyncSuccess;

   path.path = "instruction_test.cpp";
   operations.push_back( path );
   path.path = "dfile_0.dat";
   operations.push_back( path );
   path.path = "screenshot.png";
   operations.push_back( path );

   std::vector<worker_test::op_attrs_t>::iterator pathIt = operations.begin();
   for (; pathIt != operations.end(); pathIt++ )
   {
      pathIt->status = clientNode.sync(
         LocalResourcePath( pathIt->path ),
         RemoteResourcePath( pathIt->path ) );

      EXPECT_EQ( kRsyncSuccess, pathIt->status );
   }

   pathIt = operations.begin();
   for (; pathIt != operations.end(); pathIt++ )
   {
      coral::log::status( "Waiting for %s\n", pathIt->path.c_str() );
      if ( pathIt->status == kRsyncSuccess ) EXPECT_EQ( true, clientCallback.mSem.take() );
      coral::log::status( "Checking %s\n", pathIt->path.c_str() );
      EXPECT_EQ( true, CheckEqual(
         boost::filesystem::path( REMOTE_ROOT ) / pathIt->path,
         boost::filesystem::path( LOCAL_ROOT ) / pathIt->path ) );

      coral::log::status( "Done checking %s\n", pathIt->path.c_str() );
   }

   ASSERT_EQ( true, clientRouter.unsubscribe(RSYNC_SUB_ID, &clientNode.subscriber() ) );
   ASSERT_EQ( true, serverRouter.unsubscribe(RSYNC_SUB_ID, &serverNode.subscriber()) );

   serverRouter.cancel(true);
   clientRouter.cancel(true);
}

#endif  // WORKER_TEST_CPP

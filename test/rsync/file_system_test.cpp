#ifndef  FILE_SYSTEM_TEST_CPP
#define  FILE_SYSTEM_TEST_CPP

#include <boost/current_function.hpp>
#include <string.h>
#include "Log.h"
#include "FileSystemInterface.h"
#include "gtest/gtest.h"
#include "rsync_test_helper.h"

using namespace coral;
using namespace coral::rsync;
using namespace rsync_test_helper;

class FileSystemInterfaceTest : public ::testing::Test {
public:

   FileSystemInterfaceTest()
   {
      TEST_ROOT = boost::filesystem::current_path() / "file_sys_test_root";
      NON_EXISTENT_TEST_ROOT = boost::filesystem::current_path() / "non_existent_dir";
   }

   boost::filesystem::path TEST_ROOT;
   boost::filesystem::path NON_EXISTENT_TEST_ROOT;

protected:

   void SetUp()
   {
      coral::log::level( coral::log::Verbose );
      DeleteContents( TEST_ROOT.c_str() );
   }

   void TearDown()
   {
      coral::log::flush();
   }
};

TEST_F( FileSystemInterfaceTest, ExistTouchDelete ) {

   const std::string TEST_FILE( "test_file.txt" );

   FileSystemInterface file_sys;

   EXPECT_EQ( false, file_sys.exists( TEST_FILE ) );
   EXPECT_EQ( false, file_sys.setRoot( NON_EXISTENT_TEST_ROOT ) );

   EXPECT_EQ( true, file_sys.setRoot( TEST_ROOT ) );
   EXPECT_EQ( false, file_sys.exists( TEST_FILE ) );

   EXPECT_EQ( true, file_sys.touch( TEST_FILE ) );
   EXPECT_EQ( true, file_sys.exists( TEST_FILE ) );

   EXPECT_EQ( true, file_sys.remove( TEST_FILE ) );
   EXPECT_EQ( false, file_sys.exists( TEST_FILE ) );
}

TEST_F( FileSystemInterfaceTest, OpenWriteReadDelete ) {

   const std::string TEST_FILE( "test_file.txt" );
   const std::string TEST_STRING( "This is a test." );

   FileSystemInterface file_sys;

   EXPECT_EQ( true, file_sys.setRoot( TEST_ROOT ) );
   EXPECT_EQ( false, file_sys.exists( TEST_FILE ) );

   std::ofstream out_stream;
   EXPECT_EQ( true, file_sys.open( TEST_FILE, out_stream ) );

   out_stream << TEST_STRING << std::endl;
   EXPECT_EQ( true, out_stream.good() );

   out_stream.close();



   std::ifstream in_stream;
   EXPECT_EQ( true, file_sys.open( TEST_FILE, in_stream ) );

   char read_buffer[ 80 ] = {0};
   EXPECT_EQ( false, in_stream.eof() );
   in_stream.getline( read_buffer, sizeof( read_buffer ) );
   EXPECT_STREQ( TEST_STRING.c_str(), read_buffer );
   in_stream.close();

   EXPECT_EQ( true, file_sys.remove( TEST_FILE ) );
   EXPECT_EQ( false, file_sys.exists( TEST_FILE ) );
}

TEST_F( FileSystemInterfaceTest, StageFile ) {

   const std::string TEST_FILE( "test_file.txt" );

   FileSystemInterface file_sys;

   EXPECT_EQ( true, file_sys.setRoot( TEST_ROOT ) );
   EXPECT_EQ( false, file_sys.exists( TEST_FILE ) );

   std::ofstream out_stream;
   boost::filesystem::path stage_filename;
   EXPECT_EQ( true, file_sys.stage( TEST_FILE, stage_filename, out_stream ) );
   EXPECT_STREQ( stage_filename.c_str(), ".rsync_stage/1_stage.test_file.txt" );
   EXPECT_EQ( true, out_stream.is_open() );
   out_stream.close();

   EXPECT_EQ( true, file_sys.remove( stage_filename ) );
   EXPECT_EQ( false, file_sys.exists( stage_filename ) );
}

TEST_F( FileSystemInterfaceTest, SwapFiles ) {

   const std::string TEST_FILE( "test_file.txt" );

   FileSystemInterface file_sys;

   EXPECT_EQ( true, file_sys.setRoot( TEST_ROOT ) );
   
   // Create a file a write some data to it.
   std::ofstream out_stream;
   EXPECT_EQ( true, file_sys.open( TEST_FILE, out_stream ) );
   EXPECT_EQ( true, out_stream.is_open() );
   out_stream << "This is the main file." << std::endl;
   out_stream.close();

   // Create a swap file.
   boost::filesystem::path stage_filename;
   EXPECT_EQ( true, file_sys.stage( TEST_FILE, stage_filename, out_stream ) );
   EXPECT_STREQ( stage_filename.c_str(), ".rsync_stage/1_stage.test_file.txt" );
   EXPECT_EQ( true, out_stream.is_open() );
   out_stream << "This is the swap file." << std::endl;
   out_stream.close();

   // Swap the two files.
   EXPECT_EQ( true, file_sys.swap( TEST_FILE, stage_filename ) );

   // Verify that the main file now contains the contents from the swap file
   // and vice versa.
   char read_buffer[ 80 ];
   std::ifstream in_stream;

   memset( read_buffer, 0, sizeof( read_buffer ) );
   EXPECT_EQ( true, file_sys.open( TEST_FILE, in_stream ) );
   in_stream.getline( read_buffer, sizeof( read_buffer ) );
   EXPECT_STREQ( "This is the swap file.", read_buffer );
   in_stream.close();

   memset( read_buffer, 0, sizeof( read_buffer ) );
   EXPECT_EQ( true, file_sys.open( stage_filename, in_stream ) );
   in_stream.getline( read_buffer, sizeof( read_buffer ) );
   EXPECT_STREQ( "This is the main file.", read_buffer );
   in_stream.close();
}

#endif // FILE_SYSTEM_TEST_CPP

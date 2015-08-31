#include <boost/current_function.hpp>
#include <string.h>
#include "Log.h"
#include "Instruction.h"
#include "InstructionList.h"
#include "gtest/gtest.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

class InstructionTest : public ::testing::Test {
public:

   InstructionTest()
   {
   }

   boost::filesystem::path LOCAL_ROOT;
   boost::filesystem::path REMOTE_ROOT;


protected:

   void SetUp()
   {
      liber::log::level( liber::log::Verbose );
   }

   void TearDown()
   {
      liber::log::flush();
   }
};

TEST_F( InstructionTest, TestInstructionContainer )
{
   InstructionContainer sendContainer( EndInstruction::Type );
   EXPECT_EQ( EndInstruction::Type, sendContainer.type() );

   SerialStream stream;
   sendContainer.serialize( stream );

   InstructionContainer recvContainer;
   EXPECT_EQ( -1, recvContainer.type() );

   EXPECT_EQ( true, recvContainer.deserialize( stream ) );
   EXPECT_EQ( EndInstruction::Type, recvContainer.type() );
}

TEST_F( InstructionTest, TestBeginInstruction )
{
   boost::filesystem::path source( "invalid_dir/s_inst.cpp" );
   boost::filesystem::path destination( "d_inst.cpp" );

   JobDescriptor descriptor;
   descriptor.setSource( LocalResourcePath( source ) );
   descriptor.setDestination( LocalResourcePath( destination ) );
   EXPECT_EQ( true, descriptor.isValid() );

   BeginInstruction     sendInstruction( descriptor );
   InstructionContainer sendContainer( sendInstruction.type() );
   EXPECT_EQ( BeginInstruction::Type, sendContainer.type() );

   sendContainer.serialize( sendContainer.stream() );
   sendInstruction.serialize( sendContainer.stream() );

   std::string packed_container = sendContainer.stream().stream.str();
   EXPECT_EQ( false, packed_container.empty() );

   InstructionContainer recvContainer;
   EXPECT_EQ( -1, recvContainer.type() );
   recvContainer.stream().assign( packed_container.data(), packed_container.size() );

   EXPECT_EQ( true, recvContainer.deserialize( recvContainer.stream() ) );
   EXPECT_EQ( BeginInstruction::Type, recvContainer.type() );

   BeginInstruction recvInstruction;
   EXPECT_EQ( true, recvInstruction.deserialize( recvContainer.stream() ) );

   EXPECT_EQ( source.string(), recvInstruction.descriptor().getSourcePath().string() );
   EXPECT_EQ( destination.string(), recvInstruction.descriptor().getDestinationPath().string() );
}


TEST_F( InstructionTest, TestSegmentInstruction )
{
   Segment::ID segment_id = 15;
   SegmentInstruction sendInstruction( segment_id );
   InstructionContainer sendContainer( sendInstruction.type() );
   EXPECT_EQ( SegmentInstruction::Type, sendContainer.type() );

   sendContainer.serialize( sendContainer.stream() );
   sendInstruction.serialize( sendContainer.stream() );

   std::string packed_container = sendContainer.stream().stream.str();
   EXPECT_EQ( false, packed_container.empty() );

   InstructionContainer recvContainer;
   EXPECT_EQ( -1, recvContainer.type() );
   recvContainer.stream().assign( packed_container.data(), packed_container.size() );

   EXPECT_EQ( true, recvContainer.deserialize( recvContainer.stream() ) );
   EXPECT_EQ( SegmentInstruction::Type, recvContainer.type() );

   SegmentInstruction recvInstruction;
   EXPECT_EQ( true, recvInstruction.deserialize( recvContainer.stream() ) );

   EXPECT_EQ( segment_id, recvInstruction.segment_id_ );
}


TEST_F( InstructionTest, TestChunkInstruction )
{
   ui8 buffer[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

   ChunkInstruction sendInstruction( sizeof( buffer ) );
   memcpy( sendInstruction.data(), buffer, sizeof( buffer ) );
   InstructionContainer sendContainer( sendInstruction.type() );
   EXPECT_EQ( ChunkInstruction::Type, sendContainer.type() );

   sendContainer.serialize( sendContainer.stream() );
   sendInstruction.serialize( sendContainer.stream() );

   std::string packed_container = sendContainer.stream().stream.str();
   EXPECT_EQ( false, packed_container.empty() );

   InstructionContainer recvContainer;
   EXPECT_EQ( -1, recvContainer.type() );
   recvContainer.stream().assign( packed_container.data(), packed_container.size() );

   EXPECT_EQ( true, recvContainer.deserialize( recvContainer.stream() ) );
   EXPECT_EQ( ChunkInstruction::Type, recvContainer.type() );

   ChunkInstruction recvInstruction;
   EXPECT_EQ( true, recvInstruction.deserialize( recvContainer.stream() ) );

   EXPECT_EQ( sizeof(buffer), recvInstruction.chunk_size_bytes_ );
   EXPECT_EQ( 0, memcmp( recvInstruction.chunk_data_ptr_, buffer, sizeof( buffer ) ) );
}

TEST_F( InstructionTest, TestEndInstructionSuccess )
{
   EndInstruction sendInstruction;
   InstructionContainer sendContainer( sendInstruction.type() );
   EXPECT_EQ( EndInstruction::Type, sendContainer.type() );

   sendContainer.serialize( sendContainer.stream() );
   sendInstruction.serialize( sendContainer.stream() );

   std::string packed_container = sendContainer.stream().stream.str();
   EXPECT_EQ( false, packed_container.empty() );

   InstructionContainer recvContainer;
   EXPECT_EQ( -1, recvContainer.type() );
   recvContainer.stream().assign( packed_container.data(), packed_container.size() );

   EXPECT_EQ( true, recvContainer.deserialize( recvContainer.stream() ) );
   EXPECT_EQ( EndInstruction::Type, recvContainer.type() );

   EndInstruction recvInstruction;
   EXPECT_EQ( true, recvInstruction.deserialize( recvContainer.stream() ) );

   EXPECT_EQ( false, recvInstruction.canceled() );
   EXPECT_EQ( RsyncSuccess, recvInstruction.canceled() );
}

TEST_F( InstructionTest, TestEndInstructionCancel )
{
   EndInstruction sendInstruction;
   sendInstruction.cancel( kRsyncDestinationNotWritable );

   InstructionContainer sendContainer( sendInstruction.type() );
   EXPECT_EQ( EndInstruction::Type, sendContainer.type() );

   sendContainer.serialize( sendContainer.stream() );
   sendInstruction.serialize( sendContainer.stream() );

   std::string packed_container = sendContainer.stream().stream.str();
   EXPECT_EQ( false, packed_container.empty() );

   InstructionContainer recvContainer;
   EXPECT_EQ( -1, recvContainer.type() );
   recvContainer.stream().assign( packed_container.data(), packed_container.size() );

   EXPECT_EQ( true, recvContainer.deserialize( recvContainer.stream() ) );
   EXPECT_EQ( EndInstruction::Type, recvContainer.type() );

   EndInstruction recvInstruction;
   EXPECT_EQ( true, recvInstruction.deserialize( recvContainer.stream() ) );

   EXPECT_EQ( true, recvInstruction.canceled() );
   EXPECT_EQ( kRsyncDestinationNotWritable, recvInstruction.error() );
}


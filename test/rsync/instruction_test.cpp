#ifndef  INSTRUCTION_TEST_CPP
#define  INSTRUCTION_TEST_CPP

#include <boost/current_function.hpp>
#include <string.h>
#include "Log.h"
#include "Instruction.h"
#include "InstructionList.h"
#include "gtest/gtest.h"

using namespace coral;
using namespace coral::rsync;
using namespace coral::netapp;

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
      coral::log::level( coral::log::Verbose );
   }

   void TearDown()
   {
      coral::log::flush();
   }
};

TEST_F( InstructionTest, TestBeginInstruction ) {
   boost::filesystem::path source( "invalid_dir/s_inst.cpp" );
   boost::filesystem::path destination( "d_inst.cpp" );

   JobDescriptor descriptor;
   descriptor.setSource( LocalResourcePath( source ) );
   descriptor.setDestination( LocalResourcePath( destination ) );
   EXPECT_EQ( true, descriptor.isValid() );

   BeginInstruction send_instruction( descriptor );
   EXPECT_EQ( true, send_instruction.valid() );
   EXPECT_EQ( BeginInstruction::Type, send_instruction.type() );

   RawInstructionPtr send_instruction_ptr = send_instruction.instruction();

   // Decouple the original BeginInstruction and its raw instruction.
   // send_instruction.release();
   // EXPECT_EQ( NULL, send_instruction.instruction() );
   // EXPECT_EQ( false, send_instruction.valid() );

   EXPECT_EQ( true, send_instruction_ptr->valid() );
   EXPECT_EQ( BeginInstruction::Type, send_instruction_ptr->type() );


   BeginInstruction recv_instruction( send_instruction_ptr );
   // EXPECT_NE( NULL, recv_instruction.instruction() );
   EXPECT_EQ( true, recv_instruction.valid() );

   JobDescriptor recv_descriptor( send_instruction_ptr->payload_ptr(), send_instruction_ptr->payload_length());
   EXPECT_EQ( true, recv_instruction.descriptor( recv_descriptor ) );
   EXPECT_EQ( source.string(), recv_descriptor.getSourcePath().string() );
   EXPECT_EQ( destination.string(), recv_descriptor.getDestinationPath().string() );
}


TEST_F( InstructionTest, TestSegmentInstruction )
{
   Segment::ID segment_id = 15;
   SegmentInstruction send_instruction( segment_id );
   EXPECT_EQ( true, send_instruction.valid() );
   EXPECT_EQ( SegmentInstruction::Type, send_instruction.type() );

   RawInstructionPtr send_instruction_ptr = send_instruction.instruction();

   // Decouple the original BeginInstruction and its raw instruction.
   // send_instruction.release();
   // EXPECT_EQ( NULL, send_instruction.instruction() );
   // EXPECT_EQ( false, send_instruction.valid() );

   EXPECT_EQ( true, send_instruction_ptr->valid() );
   EXPECT_EQ( SegmentInstruction::Type, send_instruction_ptr->type() );

   SegmentInstruction recv_instruction( send_instruction_ptr );
   // EXPECT_NE( NULL, recv_instruction.instruction() );
   EXPECT_EQ( true, recv_instruction.valid() );
   EXPECT_EQ( segment_id, recv_instruction.segmentId() );
}


TEST_F( InstructionTest, TestChunkInstruction ) {
   ui8 buffer[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

   ChunkInstruction send_instruction( sizeof( buffer ) );
   memcpy( send_instruction.data(), buffer, sizeof( buffer ) );
   EXPECT_EQ( true, send_instruction.valid() );
   EXPECT_EQ( ChunkInstruction::Type, send_instruction.type() );

   RawInstructionPtr send_instruction_ptr = send_instruction.instruction();

   // Decouple the original BeginInstruction and its raw instruction.
   // send_instruction.release();
   // EXPECT_EQ( NULL, send_instruction.instruction() );
   // EXPECT_EQ( false, send_instruction.valid() );

   EXPECT_EQ( true, send_instruction_ptr->valid() );
   EXPECT_EQ( ChunkInstruction::Type, send_instruction_ptr->type() );

   ChunkInstruction recv_instruction( send_instruction_ptr );
   // EXPECT_NE( NULL, recv_instruction.instruction() );
   EXPECT_EQ( true, recv_instruction.valid() );
   EXPECT_EQ( 0, memcmp( recv_instruction.data(), buffer, sizeof( buffer ) ) );
}

TEST_F( InstructionTest, TestEndInstructionSuccess ) {
   EndInstruction send_instruction;
   EXPECT_EQ( true, send_instruction.valid() );
   EXPECT_EQ( EndInstruction::Type, send_instruction.type() );

   RawInstructionPtr send_instruction_ptr = send_instruction.instruction();

   // Decouple the original BeginInstruction and its raw instruction.
   // send_instruction.release();
   // EXPECT_EQ( NULL, send_instruction.instruction() );
   // EXPECT_EQ( false, send_instruction.valid() );

   EXPECT_EQ( true, send_instruction_ptr->valid() );
   EXPECT_EQ( EndInstruction::Type, send_instruction_ptr->type() );

   EndInstruction recv_instruction( send_instruction_ptr );
   // EXPECT_NE( NULL, recv_instruction.instruction() );
   EXPECT_EQ( true, recv_instruction.valid() );
   EXPECT_EQ( EndInstruction::Type, recv_instruction.type() );
   EXPECT_EQ( false, recv_instruction.canceled() );
   EXPECT_EQ( kRsyncSuccess, recv_instruction.error() );
}

TEST_F( InstructionTest, TestEndInstructionCancel ) {
   EndInstruction send_instruction;
   send_instruction.cancel( kRsyncDestinationNotWritable );
   EXPECT_EQ( true, send_instruction.valid() );
   EXPECT_EQ( EndInstruction::Type, send_instruction.type() );

   RawInstructionPtr send_instruction_ptr = send_instruction.instruction();

   // Decouple the original BeginInstruction and its raw instruction.
   // send_instruction.release();
   // EXPECT_EQ( NULL, send_instruction.instruction() );
   // EXPECT_EQ( false, send_instruction.valid() );

   EXPECT_EQ( true, send_instruction_ptr->valid() );
   EXPECT_EQ( EndInstruction::Type, send_instruction_ptr->type() );

   EndInstruction recv_instruction( send_instruction_ptr );
   // EXPECT_NE( NULL, recv_instruction.instruction() );
   EXPECT_EQ( true, recv_instruction.valid() );
   EXPECT_EQ( EndInstruction::Type, recv_instruction.type() );
   EXPECT_EQ( true, recv_instruction.canceled() );
   EXPECT_EQ( kRsyncDestinationNotWritable, recv_instruction.error() );
}

#endif // INSTRUCTION_TEST_CPP

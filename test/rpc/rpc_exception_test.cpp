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

#ifndef  RPC_EXCEPTION_TEST_CPP
#define  RPC_EXCEPTION_TEST_CPP

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "Log.h"
#include "RpcException.h"

class RpcExceptionTest : public ::testing::Test {
public:

   RpcExceptionTest() {}

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

TEST_F( RpcExceptionTest, SerializeDeserializeTraceFrame ) {

   using namespace coral::rpc;

   TraceFrame source( "TestClass", "method", __FILE__, 1234 );
   EXPECT_STREQ( "TestClass", source.resource_name_.c_str() );
   EXPECT_STREQ( "method", source.method_name_.c_str() );
   EXPECT_STREQ( __FILE__, source.filename_.c_str() );
   EXPECT_EQ( 1234, source.line_number_ );
   
   std::string serialized = source.serialize();

   TraceFrame dest;
   EXPECT_EQ( true, dest.deserialize( serialized ) );
   EXPECT_STREQ( source.resource_name_.c_str(), dest.resource_name_.c_str() );
   EXPECT_STREQ( source.method_name_.c_str(), dest.method_name_.c_str() );
   EXPECT_STREQ( source.filename_.c_str(), dest.filename_.c_str() );
   EXPECT_EQ( source.line_number_, dest.line_number_ );
}

TEST_F( RpcExceptionTest, SerializeDeserializeException ) {

   using namespace coral::rpc;

   RpcException source;
   source.id = kUnknownMethod;
   source.message = "An error occurred.";
   source.reporter = RpcException::kServer;

   // Add a couple of trace frames.
   source.pushFrame( TraceFrame( "TestClass", "method", __FILE__, 1234 ) );
   source.pushFrame( TraceFrame( "TestClass", "other_method", __FILE__, 85 ) );

   std::string serialized = source.serialize();

   RpcException dest;
   EXPECT_EQ( true, dest.deserialize( serialized ) );
   EXPECT_EQ( source.id, dest.id );
   EXPECT_STREQ( source.message.c_str(), dest.message.c_str() );
   EXPECT_EQ( source.reporter, dest.reporter );

   const std::vector<TraceFrame>& frames = dest.frameTrace();
   EXPECT_EQ( 2, frames.size() );
   if ( frames.size() == 2 )
   {
      EXPECT_STREQ( "TestClass", frames[0].resource_name_.c_str() );
      EXPECT_STREQ( "method", frames[0].method_name_.c_str() );
      EXPECT_STREQ( __FILE__, frames[0].filename_.c_str() );
      EXPECT_EQ( 1234, frames[0].line_number_ );
      EXPECT_STREQ( "TestClass", frames[1].resource_name_.c_str() );
      EXPECT_STREQ( "other_method", frames[1].method_name_.c_str() );
      EXPECT_STREQ( __FILE__, frames[1].filename_.c_str() );
      EXPECT_EQ( 85, frames[1].line_number_ );
   }
}

#endif // RPC_EXCEPTION_TEST_CPP

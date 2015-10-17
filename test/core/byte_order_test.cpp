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

#ifndef  BYTE_ORDER_TEST_CPP
#define  BYTE_ORDER_TEST_CPP

#include "ByteOrder.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace coral::net;

class ByteSwapTest : public ::testing::Test {
public:

   ByteSwapTest() {}

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

TEST_F( ByteSwapTest, SwapTypes ) {

   i16  tempS16 = 0x0123;
   ui16 tempU16 = 0x0123;
   i32  tempS32 = 0x01234567;
   ui32 tempU32 = 0x01234567;
   i64  tempS64 = 0x0123456789ABCDEF;
   ui64 tempU64 = 0x0123456789ABCDEF;

   EXPECT_EQ( 0x2301, ByteOrder::NetSwap( tempS16 ) );
   EXPECT_EQ( 0x2301, ByteOrder::NetSwap( tempU16 ) );
   EXPECT_EQ( 0x67452301, ByteOrder::NetSwap( tempS32 ) );
   EXPECT_EQ( 0x67452301, ByteOrder::NetSwap( tempU32 ) );
   EXPECT_EQ( 0xEFCDAB8967452301, ByteOrder::NetSwap( tempS64 ) );
   EXPECT_EQ( 0xEFCDAB8967452301, ByteOrder::NetSwap( tempU64 ) );
   
}


#endif  // BYTE_ORDER_TEST_CPP

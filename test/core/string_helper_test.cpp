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

#ifndef  STRING_HELPER_TEST_CPP
#define  STRING_HELPER_TEST_CPP

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "StringHelper.h"

class StringHelperTest : public ::testing::Test {
public:

   StringHelperTest() {}

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

TEST_F( StringHelperTest, StringSplitTest ) {

   using namespace coral::helpers;

   std::string test_string = "This is a sentence where words are delimited by spaces.";

   std::vector<std::string> tokens = string_helper::split( test_string, ' ' );

   const char* EXPECTED_TOKENS[] = {
      "This", "is", "a", "sentence", "where", "words", "are", "delimited", "by", "spaces."
   };

   for (int index = 0; index < tokens.size(); ++index) {
      EXPECT_EQ( EXPECTED_TOKENS[index], tokens[index] );
   }
}

TEST_F( StringHelperTest, StringTrim ) {
   using namespace coral::helpers;

   std::string test_string = " left";
   EXPECT_STREQ( " left", string_helper::right_trim( test_string ).c_str() );
   EXPECT_STREQ( " left", test_string.c_str() );
   EXPECT_STREQ( "left", string_helper::left_trim( test_string ).c_str() );
   EXPECT_STREQ( "left", test_string.c_str() );

   test_string = "\tleft";
   EXPECT_STREQ( "\tleft", string_helper::right_trim( test_string ).c_str() );
   EXPECT_STREQ( "\tleft", test_string.c_str() );
   EXPECT_STREQ( "left", string_helper::left_trim( test_string ).c_str() );
   EXPECT_STREQ( "left", test_string.c_str() );

   test_string = "right ";
   EXPECT_STREQ( "right ", string_helper::left_trim( test_string ).c_str() );
   EXPECT_STREQ( "right ", test_string.c_str() );
   EXPECT_STREQ( "right", string_helper::right_trim( test_string ).c_str() );
   EXPECT_STREQ( "right", test_string.c_str() );

   test_string = "right\t";
   EXPECT_STREQ( "right\t", string_helper::left_trim( test_string ).c_str() );
   EXPECT_STREQ( "right\t", test_string.c_str() );
   EXPECT_STREQ( "right", string_helper::right_trim( test_string ).c_str() );
   EXPECT_STREQ( "right", test_string.c_str() );

   test_string = "  spaces on the ends    ";
   EXPECT_STREQ( "spaces on the ends", string_helper::trim( test_string ).c_str() );
   EXPECT_STREQ( "spaces on the ends", test_string.c_str() );
   EXPECT_STREQ( "spaces on the ends", string_helper::trim( test_string ).c_str() );
}

TEST_F( StringHelperTest, StringToUpperToLower ) {
   using namespace coral::helpers;

   std::string test_string = "UPPER";
   EXPECT_STREQ( "UPPER", string_helper::to_upper( test_string ).c_str() );
   EXPECT_STREQ( "upper", string_helper::to_lower( test_string ).c_str() );

   test_string = "lower";
   EXPECT_STREQ( "lower", string_helper::to_lower( test_string ).c_str() );
   EXPECT_STREQ( "LOWER", string_helper::to_upper( test_string ).c_str() );
}

#endif // STRING_HELPER_TEST_CPP

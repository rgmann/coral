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

#include <boost/current_function.hpp>
#include "Log.h"
#include "Date.h"
#include "gtest/gtest.h"

using namespace coral;
using namespace coral::cli;

class DateTimeTest : public ::testing::Test {
public:

   DateTimeTest() {}


protected:

   void SetUp()
   {
      coral::log::level( coral::log::Warn );
   }

   void TearDown()
   {
      coral::log::flush();
   }

};

TEST_F( DateTimeTest, MonthTest ) {
   EXPECT_STREQ( "January", Date::Month( 0 ) );
   EXPECT_STREQ( "February", Date::Month( 1 ) );
   EXPECT_STREQ( "March", Date::Month( 2 ) );
   EXPECT_STREQ( "April", Date::Month( 3 ) );   
   EXPECT_STREQ( "May", Date::Month( 4 ) );
   EXPECT_STREQ( "June", Date::Month( 5 ) );
   EXPECT_STREQ( "July", Date::Month( 6 ) );
   EXPECT_STREQ( "August", Date::Month( 7 ) );
   EXPECT_STREQ( "September", Date::Month( 8 ) );
   EXPECT_STREQ( "October", Date::Month( 9 ) );
   EXPECT_STREQ( "November", Date::Month( 10 ) );
   EXPECT_STREQ( "December", Date::Month( 11 ) );
}

TEST_F( DateTimeTest, DayTest ) {
   EXPECT_STREQ( "Sunday", Date::Day( 0 ) );
   EXPECT_STREQ( "Monday", Date::Day( 1 ) );
   EXPECT_STREQ( "Tuesday", Date::Day( 2 ) );
   EXPECT_STREQ( "Wednesday", Date::Day( 3 ) );   
   EXPECT_STREQ( "Thursday", Date::Day( 4 ) );
   EXPECT_STREQ( "Friday", Date::Day( 5 ) );
   EXPECT_STREQ( "Saturday", Date::Day( 6 ) );
}

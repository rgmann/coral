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
#include "InteractiveCommandRouter.h"
#include "gtest/gtest.h"

using namespace coral;
using namespace coral::cli;

class InteractiveCommandRouterTest : public ::testing::Test {
public:

   InteractiveCommandRouterTest() {}


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

TEST_F( InteractiveCommandRouterTest, InteractiveCommandWithAlias ) {
   class TestCommandWithAlias : public InteractiveCommand {
   public:

      TestCommandWithAlias() :
         InteractiveCommand( "test", "This is a test command", "test_command" ) {}

      void process( const ArgumentList& arguments ) {}
   };

   TestCommandWithAlias command;
   EXPECT_EQ( "test", command.command() );
   EXPECT_EQ( "This is a test command", command.brief() );
   EXPECT_EQ( true, command.hasAlias() );
   EXPECT_EQ( "test_command", command.alias() );
}

TEST_F( InteractiveCommandRouterTest, InteractiveCommandWithoutAlias ) {
   class TestCommandWithoutAlias : public InteractiveCommand {
   public:

      TestCommandWithoutAlias() :
         InteractiveCommand( "test", "This is a test command" ) {}

      void process( const ArgumentList& arguments ) {}
   };

   TestCommandWithoutAlias command;
   EXPECT_EQ( "test", command.command() );
   EXPECT_EQ( "This is a test command", command.brief() );
   EXPECT_FALSE( command.hasAlias() );
   EXPECT_EQ( "", command.alias() );
}

TEST_F( InteractiveCommandRouterTest, CommandParsing ) {
   class TestCommand : public InteractiveCommand {
   public:
      
      TestCommand( const std::string& name, const std::string& description ) :
         InteractiveCommand( name, description ) {}

      void process( const ArgumentList& arguments ) {
         received_args_ = arguments;
      }

      ArgumentList received_args_;
   };

   TestCommand        first_command( "first", "This is test command");
   TestCommand        second_command( "second", "This is another test command");
   TestCommand        third_command( "first", "This is yet another test command");
   InteractiveCommandRouter router;

   EXPECT_EQ( true, router.add( &first_command ) );
   EXPECT_EQ( true, router.add( &second_command ) );
   EXPECT_FALSE( router.add( &third_command ) );

   std::stringstream stream;
   stream << "second arg1 arg2 arg3\n";
   router.processLine( stream );
   EXPECT_EQ( true, first_command.received_args_.empty() );
   EXPECT_FALSE( second_command.received_args_.empty() );
   EXPECT_EQ( 3u, second_command.received_args_.size() );
   EXPECT_EQ( "arg1", second_command.received_args_[ 0 ] );
   EXPECT_EQ( "arg2", second_command.received_args_[ 1 ] );
   EXPECT_EQ( "arg3", second_command.received_args_[ 2 ] );
   EXPECT_EQ( true, third_command.received_args_.empty() );
}

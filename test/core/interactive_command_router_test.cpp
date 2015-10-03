#include <boost/current_function.hpp>
#include "Log.h"
#include "InteractiveCommandRouter.h"
#include "gtest/gtest.h"

using namespace liber;
using namespace liber::cli;

class InteractiveCommandRouterTest : public ::testing::Test {
public:

   InteractiveCommandRouterTest() {}


protected:

   void SetUp()
   {
      liber::log::level( liber::log::Warn );
   }

   void TearDown()
   {
      liber::log::flush();
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
   EXPECT_EQ( false, command.hasAlias() );
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
   EXPECT_EQ( false, router.add( &third_command ) );

   std::stringstream stream;
   stream << "second arg1 arg2 arg3\n";
   router.processLine( stream );
   EXPECT_EQ( true, first_command.received_args_.empty() );
   EXPECT_EQ( false, second_command.received_args_.empty() );
   EXPECT_EQ( 3, second_command.received_args_.size() );
   EXPECT_EQ( "arg1", second_command.received_args_[ 0 ] );
   EXPECT_EQ( "arg2", second_command.received_args_[ 1 ] );
   EXPECT_EQ( "arg3", second_command.received_args_[ 2 ] );
   EXPECT_EQ( true, third_command.received_args_.empty() );
}

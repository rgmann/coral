#include <boost/current_function.hpp>
#include "Log.h"
#include "ArgParser.h"
#include "gtest/gtest.h"

using namespace coral;
using namespace coral::cli;

class ArgParserTest : public ::testing::Test {
public:

   ArgParserTest() {}


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

TEST_F( ArgParserTest, InvalidDefinitions ) {
   EXPECT_EQ( NULL, Argument::Create("") );
   EXPECT_EQ( NULL, Argument::Create(
      "primary: p, alt: port, type: opt, \
       vtype: string, required, desc: Set port number") );
   EXPECT_EQ( NULL, Argument::Create(
      "name: Port, alt: port, type: opt, \
       vtype: string, required, desc: Set port number") );
}

TEST_F( ArgParserTest, RequiredOption ) {
   Argument* argument_ptr = Argument::Create(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: int, required, desc: Set port number" );

   EXPECT_EQ( "Port", argument_ptr->name() );
   EXPECT_EQ( "p", argument_ptr->primary() );
   EXPECT_EQ( "port", argument_ptr->alt() );
   EXPECT_EQ( "Set port number", argument_ptr->description() );
   EXPECT_EQ( Argument::ArgRequired, argument_ptr->policy() );
   EXPECT_EQ( Argument::ArgOption, argument_ptr->type() );
   EXPECT_EQ( Argument::ArgInt, argument_ptr->valueType() );

   delete argument_ptr;
}

TEST_F( ArgParserTest, OptionalOption ) {
   Argument* argument_ptr = Argument::Create(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, desc: Set port number" );

   EXPECT_EQ( "Port", argument_ptr->name() );
   EXPECT_EQ( "p", argument_ptr->primary() );
   EXPECT_EQ( "port", argument_ptr->alt() );
   EXPECT_EQ( true, argument_ptr->hasAlt() );
   EXPECT_EQ( "Set port number", argument_ptr->description() );
   EXPECT_EQ( Argument::ArgOptional, argument_ptr->policy() );
   EXPECT_EQ( Argument::ArgOption, argument_ptr->type() );
   EXPECT_EQ( Argument::ArgString, argument_ptr->valueType() );

   delete argument_ptr;
}

TEST_F( ArgParserTest, AddInvalidDefinitions ) {
   ArgParser arg_parser;

   EXPECT_FALSE( arg_parser.addArg("") );
   EXPECT_FALSE( arg_parser.addArg(
      "primary: p, alt: port, type: opt, \
       vtype: string, required, desc: Set port number"));
   EXPECT_FALSE( arg_parser.addArg(
      "name: Port, alt: port, type: opt, \
       vtype: string, required, desc: Set port number"));

   // Attempt to add the same definition twice.
   EXPECT_EQ( true, arg_parser.addArg(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, desc: Set port number" ));
   EXPECT_FALSE( arg_parser.addArg(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, desc: Set port number" ));
}

TEST_F( ArgParserTest, ParseOptionalArg ) {
   ArgParser arg_parser;

   EXPECT_EQ( true, arg_parser.addArg(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, desc: Set port number" ));

   const char* argv_1[] = { "test_program" };
   int argc_1 = 1;

   std::string port_string;
   int port_int = 0;
   EXPECT_EQ( true, arg_parser.parse( argv_1, argc_1 ) );
   EXPECT_FALSE( arg_parser.isSet( Argument::ArgName, "Port" ) );
   EXPECT_FALSE( arg_parser.getArgVal( Argument::ArgName, "Port", port_string ) );

   // Test "-p" option expects a value (it is not just a flag).
   const char* argv_2[] = { "test_program", "-p" };
   int argc_2 = 2;
   EXPECT_FALSE( arg_parser.parse( argv_2, argc_2 ) );
   EXPECT_FALSE( arg_parser.isSet( Argument::ArgName, "Port" ) );
   EXPECT_FALSE( arg_parser.getArgVal( Argument::ArgName, "Port", port_string ) );

   const char* argv_3[] = { "test_program", "-p", "4567"  };
   int argc_3 = 3;
   EXPECT_EQ( true, arg_parser.parse( argv_3, argc_3 ) );
   EXPECT_EQ( true, arg_parser.isSet( Argument::ArgName, "Port" ) );
   EXPECT_EQ( true, arg_parser.getArgVal( Argument::ArgName, "Port", port_string ) );
   EXPECT_EQ( true, arg_parser.getArgVal( Argument::ArgName, "Port", port_int ) );
   EXPECT_EQ( "4567", port_string );
   EXPECT_EQ( 4567, port_int );
}

TEST_F( ArgParserTest, ParseRequiredArg ) {
   ArgParser arg_parser;

   EXPECT_EQ( true, arg_parser.addArg(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, required, desc: Set port number" ));

   const char* argv_1[] = { "test_program" };
   int argc_1 = 1;

   std::string port_string;
   int port_int = 0;
   EXPECT_FALSE( arg_parser.parse( argv_1, argc_1 ) );
   EXPECT_FALSE( arg_parser.isSet( Argument::ArgName, "Port" ) );
   EXPECT_FALSE( arg_parser.getArgVal( Argument::ArgName, "Port", port_string ) );

   // Test "-p" option expects a value (it is not just a flag).
   const char* argv_2[] = { "test_program", "-p" };
   int argc_2 = 2;
   EXPECT_FALSE( arg_parser.parse( argv_2, argc_2 ) );
   EXPECT_FALSE( arg_parser.isSet( Argument::ArgName, "Port" ) );
   EXPECT_FALSE( arg_parser.getArgVal( Argument::ArgName, "Port", port_string ) );

   const char* argv_3[] = { "test_program", "-p", "4567" };
   int argc_3 = 3;
   EXPECT_EQ( true, arg_parser.parse( argv_3, argc_3 ) );
   EXPECT_EQ( true, arg_parser.isSet( Argument::ArgName, "Port" ) );
   EXPECT_EQ( true, arg_parser.getArgVal( Argument::ArgName, "Port", port_string ) );
   EXPECT_EQ( true, arg_parser.getArgVal( Argument::ArgName, "Port", port_int ) );
   EXPECT_EQ( "4567", port_string );
   EXPECT_EQ( 4567, port_int );

   EXPECT_EQ( true, arg_parser.addArg(
      "name: Temperature, primary: t, alt: temp, type: opt, \
      vtype: float, desc: Set the temperature" ));
   const char* argv_4[] = { "test_program", "-p", "4567", "--temp", "45.867" };
   int         argc_4 = 5;
   EXPECT_EQ( true, arg_parser.parse( argv_4, argc_4 ) );
   EXPECT_EQ( true, arg_parser.isSet( Argument::ArgName, "Port" ) );
   EXPECT_EQ( true, arg_parser.getArgVal( Argument::ArgName, "Port", port_string ) );
   EXPECT_EQ( true, arg_parser.getArgVal( Argument::ArgName, "Port", port_int ) );
   EXPECT_EQ( "4567", port_string );
   EXPECT_EQ( 4567, port_int );
   float temperature = 0.0;
   EXPECT_EQ( true, arg_parser.isSet( Argument::ArgName, "Temperature" ) );
   EXPECT_EQ( true, arg_parser.getArgVal( Argument::ArgName, "Temperature", temperature ) );
   EXPECT_FLOAT_EQ( 45.867, temperature );
}

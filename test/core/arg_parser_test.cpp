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
   EXPECT_EQ( Argument::ArgInt, argument_ptr->value_type() );

   delete argument_ptr;
}

TEST_F( ArgParserTest, OptionalOption ) {
   Argument* argument_ptr = Argument::Create(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, desc: Set port number" );

   EXPECT_EQ( "Port", argument_ptr->name() );
   EXPECT_EQ( "p", argument_ptr->primary() );
   EXPECT_EQ( "port", argument_ptr->alt() );
   EXPECT_EQ( true, argument_ptr->has_alt() );
   EXPECT_EQ( "Set port number", argument_ptr->description() );
   EXPECT_EQ( Argument::ArgOptional, argument_ptr->policy() );
   EXPECT_EQ( Argument::ArgOption, argument_ptr->type() );
   EXPECT_EQ( Argument::ArgString, argument_ptr->value_type() );

   delete argument_ptr;
}

TEST_F( ArgParserTest, AddInvalidDefinitions ) {
   ArgParser arg_parser;

   EXPECT_FALSE( arg_parser.add_arg("") );
   EXPECT_FALSE( arg_parser.add_arg(
      "primary: p, alt: port, type: opt, \
       vtype: string, required, desc: Set port number"));
   EXPECT_FALSE( arg_parser.add_arg(
      "name: Port, alt: port, type: opt, \
       vtype: string, required, desc: Set port number"));

   // Attempt to add the same definition twice.
   EXPECT_EQ( true, arg_parser.add_arg(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, desc: Set port number" ));
   EXPECT_FALSE( arg_parser.add_arg(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, desc: Set port number" ));
}

TEST_F( ArgParserTest, ParseOptionalArg ) {
   ArgParser arg_parser;

   EXPECT_EQ( true, arg_parser.add_arg(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, desc: Set port number" ));

   const char* argv_1[] = { "test_program" };
   int argc_1 = 1;

   std::string port_string;
   int port_int = 0;
   EXPECT_EQ( true, arg_parser.parse( argv_1, argc_1 ) );
   EXPECT_FALSE( arg_parser.is_set( Argument::ArgName, "Port" ) );
   EXPECT_FALSE( arg_parser.get_arg_val( Argument::ArgName, "Port", port_string ) );

   // Test "-p" option expects a value (it is not just a flag).
   const char* argv_2[] = { "test_program", "-p" };
   int argc_2 = 2;
   EXPECT_FALSE( arg_parser.parse( argv_2, argc_2 ) );
   EXPECT_FALSE( arg_parser.is_set( Argument::ArgName, "Port" ) );
   EXPECT_FALSE( arg_parser.get_arg_val( Argument::ArgName, "Port", port_string ) );

   const char* argv_3[] = { "test_program", "-p", "4567"  };
   int argc_3 = 3;
   EXPECT_EQ( true, arg_parser.parse( argv_3, argc_3 ) );
   EXPECT_EQ( true, arg_parser.is_set( Argument::ArgName, "Port" ) );
   EXPECT_EQ( true, arg_parser.get_arg_val( Argument::ArgName, "Port", port_string ) );
   EXPECT_EQ( true, arg_parser.get_arg_val( Argument::ArgName, "Port", port_int ) );
   EXPECT_EQ( "4567", port_string );
   EXPECT_EQ( 4567, port_int );
}

TEST_F( ArgParserTest, ParseRequiredArg ) {
   ArgParser arg_parser;

   EXPECT_EQ( true, arg_parser.add_arg(
      "name: Port, primary: p, alt: port, type: opt, \
      vtype: string, required, desc: Set port number" ));

   const char* argv_1[] = { "test_program" };
   int argc_1 = 1;

   std::string port_string;
   int port_int = 0;
   EXPECT_FALSE( arg_parser.parse( argv_1, argc_1 ) );
   EXPECT_FALSE( arg_parser.is_set( Argument::ArgName, "Port" ) );
   EXPECT_FALSE( arg_parser.get_arg_val( Argument::ArgName, "Port", port_string ) );

   // Test "-p" option expects a value (it is not just a flag).
   const char* argv_2[] = { "test_program", "-p" };
   int argc_2 = 2;
   EXPECT_FALSE( arg_parser.parse( argv_2, argc_2 ) );
   EXPECT_FALSE( arg_parser.is_set( Argument::ArgName, "Port" ) );
   EXPECT_FALSE( arg_parser.get_arg_val( Argument::ArgName, "Port", port_string ) );

   const char* argv_3[] = { "test_program", "-p", "4567" };
   int argc_3 = 3;
   EXPECT_EQ( true, arg_parser.parse( argv_3, argc_3 ) );
   EXPECT_EQ( true, arg_parser.is_set( Argument::ArgName, "Port" ) );
   EXPECT_EQ( true, arg_parser.get_arg_val( Argument::ArgName, "Port", port_string ) );
   EXPECT_EQ( true, arg_parser.get_arg_val( Argument::ArgName, "Port", port_int ) );
   EXPECT_EQ( "4567", port_string );
   EXPECT_EQ( 4567, port_int );

   EXPECT_EQ( true, arg_parser.add_arg(
      "name: Temperature, primary: t, alt: temp, type: opt, \
      vtype: float, desc: Set the temperature" ));
   const char* argv_4[] = { "test_program", "-p", "4567", "--temp", "45.867" };
   int         argc_4 = 5;
   EXPECT_EQ( true, arg_parser.parse( argv_4, argc_4 ) );
   EXPECT_EQ( true, arg_parser.is_set( Argument::ArgName, "Port" ) );
   EXPECT_EQ( true, arg_parser.get_arg_val( Argument::ArgName, "Port", port_string ) );
   EXPECT_EQ( true, arg_parser.get_arg_val( Argument::ArgName, "Port", port_int ) );
   EXPECT_EQ( "4567", port_string );
   EXPECT_EQ( 4567, port_int );
   float temperature = 0.0;
   EXPECT_EQ( true, arg_parser.is_set( Argument::ArgName, "Temperature" ) );
   EXPECT_EQ( true, arg_parser.get_arg_val( Argument::ArgName, "Temperature", temperature ) );
   EXPECT_FLOAT_EQ( 45.867, temperature );
}

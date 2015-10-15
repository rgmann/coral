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


#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>
#include <vector>
#include <map>
#include "Argument.h"

namespace coral {
namespace cli {

class ArgParser {
public:

   ///
   /// Default constructor
   ///
   ArgParser();

   ///
   /// Default destructor
   ///
   ~ArgParser();
   
   ///
   /// Define a new argument to parse from the command line. All arguments
   /// should be defined before invoking parse() on the command line.
   ///
   /// Arguments are defined using a set of comma-separated key-value
   /// ("key : value"), or key-only ("key"), attributes. The following
   /// attributes are available:
   ///
   ///   "name : <name>"       - Specify the name of an argument. The name is
   ///                           used for querying the argument after parsing.
   ///                           [REQUIRED][UNIQUE]
   ///
   ///   "primary : <primary>" - Specifies the primary command line argument
   ///                           identifier. For example, defining an argument
   ///                           with "primary : p" will match "-p" at the
   ///                           command line.
   ///                           [REQUIRED if no alternate][UNIQUE]
   ///
   ///   "alt : <alt>"         - Specifies the alternate command-line argument
   ///                           identifier. For example, defining an argument
   ///                           with "secondary : port" will match "--port" at
   ///                           at the command line.
   ///                           The "help" alternate is reserved.
   ///                           [REQUIRED if no primary][UNIQUE]
   ///
   ///   "desc[description] : <description>" - Specify a brief argument
   ///                           description that does not contain commas or
   ///                           colons.
   ///
   ///   "type : <flag|[opt|option]>" - Specifies whether this argument is a
   ///                           flag (e.g. a "-v" flag to enable verbose
   ///                           output) or an option (e.g. "-p 2000" specifies
   ///                           port 2000).  If the type is "opt" or "option",
   ///                           then the following command line argument
   ///                           (separated by white space) is assumed to be
   ///                           the option value. By default, an argument is
   ///                           assumed to be a "option".
   ///
   ///   "required"            - By default, arguments are not required.
   ///                           However, if "required" is specified in an
   ///                           argument's definition, then parse() will fail
   ///                           if the argument missing from the command line.
   ///                           Ignored for flag-type arguments.
   ///
   ///   "valuetype[vtype] : <value type>" - Specifies the expected type of
   ///                           associated with an option-type argument. Valid
   ///                           types include "int"/"integer", "float", or
   ///                           "string". By default the value type is
   ///                           "string". Specifying a value type in the
   ///                           argument definition will cause parse() to
   ///                           validate the type and return false if a value
   ///                           of the specified type cannot be parsed from
   ///                           the command line. If you wish to bypass this
   ///                           validation, specify a value type of "string"
   ///                           or do not specify a value type at all.
   ///
   ///   "def[default] : <default value>" - Specifies the default value for
   ///                           for optional, option-type arguments.
   ///
   /// @param  definition  Argument definition string
   /// @return bool        True if an argument was successfully added from the
   ///                     specified definition; false if the definition could
   ///                     not be parsed, or if a command with the same name,
   ///                     primary, or alternate IDs.
   ///
   bool  addArg( const std::string& definition );
   
   ///
   /// Parse the command line arguments. If "--help" at any point in "argv",
   /// then parse() returns false and helpRequested() returns true.
   ///
   /// @param  argv  Array of command line argument tokens
   /// @param  argc  Number of tokens in argv
   /// @return bool  True if all command-line tokens were successfully parsed
   ///               and all required arguments were found; false on parse
   ///               failure or if "--help" was specified.
   ///
   bool parse( const char* argv[], int argc );

   ///
   /// Check whether "--help" was specified at the command line. Due to varying
   /// use cases, it is the caller's responsibility to get and print the help
   /// message (see printHelp()).
   ///
   /// @return bool  True if the help message was requested at the command
   ///               line; false otherwise.
   ///
   bool helpRequested() const;
   
   ///
   /// Retrieve a string containing all argument parsing errors. If there are
   /// no errors, this will be an empty string.
   ///
   /// @param  print_help  True if the help message (printed by --help flag)
   ///                     should be appended to the error message string.
   /// @return std::string Error message string
   ///
   std::string printArgErrors( bool print_help = false );
   
   ///
   /// Retrieve a string containing the a description of all defined
   /// arguments.
   ///
   /// @return std::string  Help message string
   ///
   std::string printHelp();
   
   ///
   /// Query whether a specified argument was specified at the command line.
   ///
   /// @param  arg_attr  Argument attribute to query on
   ///                   (ArgPrimary, ArgAlt, or ArgName)
   /// @param  arg_attr_val  Value of the argument's arg_attr attribute
   /// @return bool      True if the argument was specified at the command
   ///                   line; false if it was not specified or the specified
   ///                   argument cannot be found.
   ///
   bool isSet( Argument::ArgField arg_attr, const std::string &arg_attr_val );

   ///
   /// Get the value of the specified argument as a 32-bit integer.
   ///
   /// @param  arg_attr  Argument attribute to query on
   ///                   (ArgPrimary, ArgAlt, or ArgName)
   /// @param  arg_attr_val  Value of the argument's arg_attr attribute
   /// @param  field_val Field value
   /// @return bool      True if the option-type argument exists and a 32-bit
   ///                   integer can parsed from its value; false otherwise.
   ///
   bool getArgVal( Argument::ArgField arg_attr, 
                   const std::string& arg_attr_val,
                   i32&               field_val );

   ///
   /// Get the value of the specified argument as a single-precision floating
   /// point value.
   ///
   /// @param  arg_attr  Argument attribute to query on
   ///                   (ArgPrimary, ArgAlt, or ArgName)
   /// @param  arg_attr_val  Value of the argument's arg_attr attribute
   /// @param  field_val Field value
   /// @return bool      True if the option-type argument exists and a 32-bit
   ///                   float can parsed from its value; false otherwise.
   ///
   bool getArgVal( Argument::ArgField arg_attr, 
                   const std::string& arg_attr_val,
                   f32&               field_val );

   ///
   /// Get the value of the specified argument as a string.
   ///
   /// @param  arg_attr  Argument attribute to query on
   ///                   (ArgPrimary, ArgAlt, or ArgName)
   /// @param  arg_attr_val  Value of the argument's arg_attr attribute
   /// @param  field_val Field value
   /// @return bool      True if the option-type argument exists and a 32-bit
   ///                   float can parsed from its value; false otherwise.
   ///
   bool getArgVal( Argument::ArgField arg_attr, 
                   const std::string& arg_attr_val,
                   std::string&       field_val );

private:

   //
   // Unimplemented copy constructor and copy operator
   //
   ArgParser( const ArgParser& );
   ArgParser& operator= ( const ArgParser& );

   ///
   /// Find the Argument instance associated with a specified command-line
   /// argument token. The token is expected to be in the form "-o" or
   /// "--option".
   ///
   /// @param  argument_str  Command-line argument token
   /// @return Argument*     True if an Argument was found; NULL otherwise.
   ///
   Argument* findArgument( const std::string& argument_str );

   ///
   /// Check whether all Arguments flagged as required were found at the
   /// command line.
   ///
   /// @return  bool  True if all required arguments were found;
   ///                false otherwise.
   ///
   bool allRequiredArgsFound();
   
   ///
   /// Retrieve a defined argument based its primary ID, alternate ID, or
   /// its name.
   ///
   /// @param  arg_attr  Argument attribute to when retrieving the Argument
   /// @param  arg_attr_val  Argument attribute value
   /// @return Argument* Pointer to argument or NULL if the requested argument
   ///                   was not found.
   ///
   Argument* getArgByField( Argument::ArgField arg_attr, 
                            const std::string& arg_attr_val );

   ///
   /// Log a command-line parsing error.
   ///
   /// @param  arg_id  Argument ID (primary or alternate) as specified at the
   ///                 command line
   /// @param  error   Error message
   ///
   void logError( const std::string& arg_id, const std::string& error );

   ///
   /// Clear the error log.
   ///
   void resetErrorLog();

   ///
   /// Retrieve the number of logged errors.
   ///
   int errorCount();
   
   ///
   /// Reset the state of all arguments to the unparsed state.
   ///
   void resetArgs();

   ///
   /// Check whether the specified value type can be parsed from the specified
   /// string.
   ///
   /// @param  type  Value type
   /// @param  value Value string
   ///
   static bool ValidType(Argument::ArgValType type, const std::string& value );
   
   
private:

   // Typedefs to improve readability
   typedef std::map<std::string,Argument*> ArgumentMap;
   typedef std::vector<Argument*>          ArgumentList;
   typedef std::vector<std::string>        ArgumentErrorList;
   
   // Map between Argument primary ID and Argument object
   ArgumentMap       primary_table_;

   // Map between Argument alternate ID and Argument object
   ArgumentMap       alt_table_;

   // Map between Argument name and Argument object
   ArgumentMap       name_table_;

   // Complete list of defined arguments
   ArgumentList      arguments_;

   // Error log
   ArgumentErrorList argument_errors_;

   // True if "--help" was specified anywhere at the command line
   bool help_requested_;
};

} // namespace cli
} // namespace coral

#endif // ARG_PARSER_H
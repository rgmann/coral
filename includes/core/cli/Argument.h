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



#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <string>
#include "BaseTypes.h"

namespace coral {
namespace cli {

class Argument {
public:
   
   enum ArgType {
      ArgFlag,
      ArgOption
   };
   
   enum ArgValType {
      ArgInt,
      ArgString,
      ArgFloat
   };
   
   enum ArgField {
      ArgPrimary,
      ArgAlt,
      ArgName
   };
   
   enum ArgPolicy {
      ArgOptional,
      ArgRequired
   };
   
   ///
   /// Default constructor.
   ///
   Argument();
   
   ///
   /// Construct a new argument to parse from the command line.
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
   /// @return Argument*   Pointer to new Argument instance if an argument was
   ///                     successfully parse from the specified definition;
   ///                     NULL if the definition could not be parsed.
   ///
   static Argument* Create(const std::string &definition);

   //
   // Accessors and mutators to get/set the Argument's attributes
   //

   void        name( const std::string& val ){ name_ = val; };
   std::string name() const { return name_; };
   
   void        primary( const std::string& val ){ id_ = val; };
   std::string primary() const { return id_; };
   
   void        alt( const std::string& val){ alternate_id_ = val; };
   std::string alt() const { return alternate_id_; };
   bool        hasAlt() const { return ( alternate_id_.empty() == false ); }
   
   void        description(const std::string& val ){ description_ = val; };
   std::string description() const { return description_; };
   
   void        policy( ArgPolicy val ){ policy_ = val; };
   ArgPolicy   policy() const { return policy_; };
   
   void        type( ArgType val ){ type_ = val; };
   ArgType     type() const { return type_; };
   
   void        value( const std::string& val ){ value_ = val; };
   std::string value() const;
   void        clearValue() { value_.erase(); };
   
   void        valueType( ArgValType value_type ){ value_type_ = value_type; };
   ArgValType  valueType() const { return value_type_; };


private:
   
   ///
   /// Parse an argument definition from the specified string.
   ///
   /// @param  definition  Argument definition string
   ///
   bool parse( const std::string& definition );


   enum ArgStatus {

      /// The argument was omitted at the command line
      ArgOmitted,

      /// The argument was specified at the command line without a required
      /// parameter.
      ArgMissingParam,

      /// The value specified with the argument at the command line cannot
      /// be parsed to the value type configured for this Argument.
      ArgBadValue,

      /// The argument was successfully parsed from the command line.
      ArgFound

   };

   ///
   /// Set command line argument parsing status for this argument. Only the
   /// ArgParser is permitted to set this status.
   ///
   /// @param  val  Command line parsing status
   ///
   void      status( ArgStatus val ){ status_ = val; };

   ///
   /// Get command line argument parsing status for this argument.
   ///
   /// @return ArgStatus  Command line parsing status for this command
   ///
   ArgStatus status(){ return status_; };

   ///
   /// Process the specified key(attribute)-value(attribute value) pair
   /// parsed from the Argument definition string.
   ///
   /// @param  attribute_name  Attribute name
   /// @param  attribute_value Attribute value (defaults to empty string)
   /// @return bool            True if a valid key-value pair was specified;
   ///                         false otherwise.
   ///
   bool setAttribute( const std::string& attribute_name, 
                      const std::string& attribute_value = "");
   
private:
   
   // Argument name
   std::string name_;

   // Argument ID (e.g. if "-p" is specified at the command line "p" is the ID)
   std::string id_;

   // Argument alternate ID (e.g. if "--port" is specified at the command line
   // "port" is the ID)
   std::string alternate_id_;

   // Brief argument description printed with the help
   std::string description_;

   // Argument type (flag or option)
   ArgType     type_;

   // Argument policy (optional or required)
   ArgPolicy   policy_;

   // Argument value type (integer, float, or string)
   ArgValType  value_type_;

   // Value parsed from command line for this argument. The value is parsed
   // and stored as a string and converted when requested.
   std::string value_;

   // For optional arguments, a default value can be configured.
   std::string default_value_;

   // Command line argument parsing status. Defaults to ArgOmmitted.
   ArgStatus   status_;

   // ArgParser is a friend so that it can set Argument parsing status
   friend class ArgParser;
};

} // namespace cli
} // namespace coral

#endif // ARGUMENT_H
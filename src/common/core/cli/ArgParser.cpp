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


#include <iostream>
#include <sstream>
#include <stdio.h>
#include "ArgParser.h"
#include "Log.h"

using namespace coral;
using namespace coral::cli;

//------------------------------------------------------------------------------
ArgParser::ArgParser()
   : help_requested_( false )
{
}

//------------------------------------------------------------------------------
ArgParser::~ArgParser()
{
   // All of the tables share pointers to Arguments held in the arguments_
   // container. Simply clear the tables and then proceed to delete destroy
   // the arguments held in the arguments_ vector.
   primary_table_.clear();
   alt_table_.clear();
   name_table_.clear();
   
   ArgumentList::iterator argument_iterator = arguments_.begin();
   for (; argument_iterator < arguments_.end(); ++argument_iterator )
   {
      Argument* argument_ptr = *argument_iterator;

      if ( argument_ptr != NULL )
      {
         delete argument_ptr;
         argument_ptr = NULL;
      }
   }
   
   arguments_.clear();
}

//------------------------------------------------------------------------------
bool ArgParser::addArg(const std::string &arg)
{
   Argument* argument_ptr = NULL;
   
   if ( ( argument_ptr = Argument::Create( arg ) ) != NULL )
   {
      // The name, primary flag, and alternate flag (if furnished) must be
      // unique.
      bool valid_argument = true;
      valid_argument &= ( name_table_.count( argument_ptr->name() ) == 0 );
      valid_argument &= ( primary_table_.count( argument_ptr->primary() ) == 0 );

      if ( argument_ptr->hasAlt() )
      {
         valid_argument &= ( alt_table_.count( argument_ptr->alt() ) == 0 );
      }


      if ( valid_argument )
      {
         arguments_.push_back( argument_ptr );

         std::pair<std::string,Argument*> primary_pair =
            std::make_pair( argument_ptr->primary(), argument_ptr );

         primary_table_.insert( primary_pair );

         if ( argument_ptr->alt().empty() == false )
         {
            std::pair<std::string,Argument*> alt_pair =
               std::make_pair( argument_ptr->alt(), argument_ptr );

            alt_table_.insert( alt_pair );
         }

         if ( argument_ptr->name().empty() == false )
         {
            std::pair<std::string,Argument*> name_pair =
               std::make_pair( argument_ptr->name(), argument_ptr );

            name_table_.insert( name_pair );
         }
      }
      else
      {
         delete argument_ptr;
         argument_ptr = NULL;
      }
   }
   
   return ( argument_ptr != NULL );
}

//------------------------------------------------------------------------------
bool ArgParser::parse( const char* arguments[], int argument_count )
{
   help_requested_ = false;

   // Reset the error log
   resetErrorLog();
   
   // Reset the arguments
   resetArgs();
   
   // Start at one to skip over program name
   for ( int argument_index = 1;
         ( argument_index < argument_count ) && ( help_requested_ == false );
         ++argument_index )
   {
      bool more_arguments_available = ( ( argument_index + 1 ) < argument_count );

      std::string argument_str( arguments[ argument_index ] );
      

      Argument* argument_ptr = findArgument( argument_str );
      
      if ( argument_ptr != NULL )
      {
         bool argument_valid = true;

         // Currently all option type args require a value to be specified.
         if ( argument_ptr->type() == Argument::ArgOption )
         {
            if ( more_arguments_available == false )
            {
               // This argument requires a value, but there are no more
               // argument tokens available.  Therefore, log error and
               // continue.
               logError( argument_str, "No value provided for option." );
               
               argument_ptr->status(Argument::ArgMissingParam);
               argument_valid = false;
            }
            else
            {
               // Set the value for the argument.
               argument_ptr->value( std::string( arguments[ ++argument_index ] ) );
               
               // Validate the parameter value type.
               if (!ValidType(argument_ptr->valueType(), argument_ptr->value()))
               {
                  // This argument requires a value, but there are no more
                  // argument tokens available.  Therefore, log error and
                  // continue.
                  logError(
                     argument_str, 
                     "Value specified for option does not have expected type."
                  );
                  
                  argument_ptr->status( Argument::ArgBadValue );
                  argument_valid = false;
               }
            }
         }
         
         if ( argument_valid )
         {
            argument_ptr->status( Argument::ArgFound );
         }
      }
   }

   bool parse_success = ( errorCount() == 0 )  &&
                        allRequiredArgsFound() &&
                        ( help_requested_ == false );

   return parse_success;
}

//------------------------------------------------------------------------------
bool ArgParser::helpRequested() const
{
   return help_requested_;
}

//------------------------------------------------------------------------------
bool ArgParser::isSet( Argument::ArgField field, const std::string &fieldStr )
{
   bool success = false;

   Argument* argument_ptr = getArgByField( field, fieldStr );

   if ( argument_ptr != NULL )
   {
      if ( argument_ptr->status() == Argument::ArgFound )
      {
         success = true;
      }
   }
   
   return success;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal(
   Argument::ArgField arg_attr,
   const std::string& arg_attr_val,
   int&               field_val )
{
   bool      success      = false;
   Argument* argument_ptr = getArgByField( arg_attr, arg_attr_val );

   if ( argument_ptr != NULL )
   {
      if ( argument_ptr->status() == Argument::ArgFound )
      {
         if ( ValidType( Argument::ArgInt, argument_ptr->value() ) )
         {
            std::stringstream stream( argument_ptr->value() );
   
            stream >> field_val;
            success = true;
         }
         else
         {
            log::error("ArgParser::getArgVal(int): Arg value does not match arg type.\n");
         }
      }
      else
      {
         log::error("ArgParser::getArgVal(int): Arg was not specified at the CL.\n");
      }
   }
   else
   {
      log::error("ArgParser::getArgVal(int): Failed to get arg by field.\n");
   }
   
   return success;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal(
   Argument::ArgField arg_attr,
   const std::string& arg_attr_val,
   f32&               field_val )
{
   bool      success      = false;
   Argument* argument_ptr = getArgByField( arg_attr, arg_attr_val );
   
   if ( argument_ptr != NULL )
   {
      if ( argument_ptr->status() == Argument::ArgFound )
      {
         if ( ValidType( Argument::ArgFloat, argument_ptr->value() ) )
         {
            std::stringstream stream(argument_ptr->value());
   
            stream >> field_val;
            success = true;
         }
         else
         {
            log::error("ArgParser::getArgVal(float): Arg value does not match arg type.\n");
         }
      }
      else
      {
         log::error("ArgParser::getArgVal(float): Arg was not specified at the CL.\n");
      }
   }
   else
   {
      log::error("ArgParser::getArgVal(float): Failed to get arg by field.\n");
   }
   
   return success;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal(
   Argument::ArgField arg_attr,
   const std::string& arg_attr_val,
   std::string&       field_val )
{
   bool      success      = false;
   Argument* argument_ptr = getArgByField( arg_attr, arg_attr_val );
   
   if ( argument_ptr != NULL )
   {
      if ( argument_ptr->status() == Argument::ArgFound )
      {
         if ( ValidType( Argument::ArgString, argument_ptr->value() ) )
         {
            // Don't need a string stream since the value is already a string.
            field_val = argument_ptr->value();
            success   = true;
         }
         else
         {
            log::error("ArgParser::getArgVal(string): Arg value does not match arg type.\n");
         }
      }
      else
      {
         log::error("ArgParser::getArgVal(string): Arg was not specified at the CL.\n");
      }
   }
   else
   {
      log::error("ArgParser::getArgVal(string): Failed to get arg by field.\n");
   }
   
   return success;
}

//------------------------------------------------------------------------------
Argument* ArgParser::getArgByField(
   Argument::ArgField arg_attr, 
   const std::string& arg_attr_val )
{
   bool found_argument = false;
   ArgumentMap::const_iterator argument_iterator;
   
   switch ( arg_attr )
   {
      case Argument::ArgPrimary:
         argument_iterator = primary_table_.find( arg_attr_val );
         found_argument    = ( argument_iterator != primary_table_.end() );
         break;
         
      case Argument::ArgAlt:
         argument_iterator = alt_table_.find( arg_attr_val );
         found_argument = ( argument_iterator != primary_table_.end() );
         break;
      
      case Argument::ArgName:
         argument_iterator = name_table_.find( arg_attr_val );
         found_argument    = ( argument_iterator != name_table_.end() );
         break;
         
      default:
         break;
   }

   Argument* argument_ptr = NULL;

   if ( found_argument )
   {
      argument_ptr = argument_iterator->second;
   }
   
   return argument_ptr;
}

//------------------------------------------------------------------------------
std::string ArgParser::printArgErrors( bool print_help_text )
{
   std::stringstream error_message;

   if ( argument_errors_.empty() == false )
   {
      ArgumentErrorList::iterator arg_error_iterator =
         argument_errors_.begin();

      error_message << "Encountered " << argument_errors_.size()
                    << " errors while parsing CLI arguments:\n";
   
      for (; arg_error_iterator < argument_errors_.end(); ++arg_error_iterator )
      {
         error_message << "   " << arg_error_iterator->c_str() << "\n";
      }
      
      if ( print_help_text == false )
      {
         error_message << "For help, specify \"-h\" or \"--help\".\n"
                       << std::endl;
      }
      else
      {
         error_message << printHelp();
      }
   }

   return error_message.str();
}

//------------------------------------------------------------------------------
std::string ArgParser::printHelp()
{
   std::stringstream help_message;

   if ( arguments_.empty() == false )
   {
      help_message << "Program options:\n";
   
      ArgumentList::iterator argument_iterator = arguments_.begin();
      for (; argument_iterator < arguments_.end(); ++argument_iterator )
      {
         Argument* argument_ptr = *argument_iterator;

         help_message << "  " << argument_ptr->name() << "[\""
                      << "-"  << argument_ptr->primary() << "\"/\""
                      << "--" << argument_ptr->alt() << "\","
                      << (( argument_ptr->policy() == Argument::ArgRequired ) ?
                                               "REQUIRED" : "OPTIONAL") << "]: "
                      << argument_ptr->description() << std::endl;
      }
   }
   else
   {
      help_message << "Program has no registered options." << std::endl;
   }

   return help_message.str();
}

//------------------------------------------------------------------------------
Argument* ArgParser::findArgument( const std::string& argument_str )
{
   Argument* argument_ptr = NULL;

   // Search secondary keys if the arg starts with "--"
   if ( argument_str.find( "--" ) == 0 )
   {
      std::string argument_id = argument_str.substr(2);
      
      // Check for reserved "help" flag.
      if ( argument_id.compare("help") == 0 )
      {
         help_requested_ = true;
      }
      else
      {
         ArgumentMap::iterator argument_iterator =
            alt_table_.find( argument_id );
         if ( argument_iterator != alt_table_.end() )
         {
            argument_ptr = argument_iterator->second;
         }
         else
         {
            // Unrecognized arg.  Log error and continue.
            logError( argument_str, "Unrecognized option." );
         }
      }
   }
   else if ( argument_str.find( "-" ) == 0 )
   {
      std::string argument_id = argument_str.substr(1);
      
      ArgumentMap::iterator argument_iterator =
         primary_table_.find( argument_id );
      if ( argument_iterator != primary_table_.end() )
      {
         argument_ptr = argument_iterator->second;
      }
      else
      {
         // Unrecognized arg.  Log error and continue.
         logError( argument_str, "Unrecognized option." );
      }
   }
   else
   {
      // All arg IDs should start with "-" or "--".
      // Therefore log an error.
      logError(argument_str, "Option flags must contain \"-\" or\"--\".");
   }

   return argument_ptr;
}

//------------------------------------------------------------------------------
bool ArgParser::allRequiredArgsFound()
{
   bool all_required_params_available = true;

   // Iterate through the arguments and check that all required arguments
   // were specified.
   std::vector<Argument*>::iterator argument_it = arguments_.begin();
   for (; argument_it < arguments_.end(); ++argument_it )
   {
      Argument* argument_ptr = *argument_it;

      if ( ( argument_ptr->policy() == Argument::ArgRequired ) &&
           ( argument_ptr->status() != Argument::ArgFound ) )
      {
         if ( all_required_params_available )
         {
            all_required_params_available = false;
         }
         
         logError( argument_ptr->primary(), "Required parameter missing." );
      }
   }

   return all_required_params_available;
}

//------------------------------------------------------------------------------
void ArgParser::logError(const std::string &optId, const std::string &msg)
{
   argument_errors_.push_back( optId + " : " + msg );
}

//------------------------------------------------------------------------------
void ArgParser::resetErrorLog()
{
   argument_errors_.clear();
}

//------------------------------------------------------------------------------
int ArgParser::errorCount()
{
   return argument_errors_.size();
}

//------------------------------------------------------------------------------
void ArgParser::resetArgs()
{
   ArgumentList::iterator argument_iterator = arguments_.begin();
   
   for (; argument_iterator < arguments_.end(); ++argument_iterator )
   {
      Argument* argument_ptr = *argument_iterator;
      
      argument_ptr->clearValue();
      argument_ptr->status(Argument::ArgOmitted);
   }
}

//------------------------------------------------------------------------------
bool ArgParser::ValidType(Argument::ArgValType type, const std::string &value)
{
   bool valid_type = false;

   std::stringstream stream( value );
   
   if ( type == Argument::ArgInt )
   {
      int test_temp = 0;
      stream >> test_temp;
      valid_type = ( stream.fail() == false );      
   }
   else if ( type == Argument::ArgFloat )
   {
      float test_float = 0;
      stream >> test_float;
      valid_type = ( stream.fail() == false );
   }
   else if ( type == Argument::ArgString )
   {
      valid_type = true;
   }
   
   return valid_type;
}

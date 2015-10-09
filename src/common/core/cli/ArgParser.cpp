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

using namespace liber::cli;

//------------------------------------------------------------------------------
ArgParser::ArgParser()
{
}

//------------------------------------------------------------------------------
ArgParser::~ArgParser()
{
   std::vector<Argument*>::iterator it = arguments_.begin();
   
   primary_table_.clear();
   alt_table_.clear();
   name_table_.clear();
   
   for (; it < arguments_.end(); ++it)
   {
      if (*it != NULL)
      {
         delete *it;
         *it = NULL;
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
bool ArgParser::parse(const char* argv[], int argc)
{
   // Reset the error log
   resetErrorLog();
   
   // Reset the arguments
   resetArgs();
   
   // Start at one to skip over program name
   for ( int argument_index = 1; argument_index < argc; ++argument_index )
   {
      Argument* argument_ptr = NULL;

      bool more_arguments_available = ( ( argument_index + 1 ) < argc );
      std::string argument_str( argv[ argument_index ] );
      
      // Search secondary keys if the arg starts with "--"
      if ( argument_str.find( "--" ) == 0 )
      {
         std::string argument_id = argument_str.substr(2);
         
         if ( alt_table_.count( argument_id ) > 0 )
         {
            argument_ptr = alt_table_[ argument_id ];
         }
         else
         {
            // Unrecognized arg.  Log error and continue.
            logError(argument_str, "Unrecognized option.");
         }
      }
      else if ( argument_str.find( "-" ) == 0 )
      {
         std::string argument_id = argument_str.substr(1);
         
         if ( primary_table_.count(argument_id) > 0 )
         {
            argument_ptr = primary_table_[ argument_id ];
         }
         else
         {
            // Unrecognized arg.  Log error and continue.
            logError(argument_str, "Unrecognized option.");
         }
      }
      else
      {
         // All arg IDs should start with "-" or "--".
         // Therefore log an error.
         logError(argument_str, "Option flags must contain \"-\" or\"--\".");
      }
      
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
               logError(argument_str, "No value provided for option.");
               
               argument_ptr->status(Argument::ArgMissingParam);
               argument_valid = false;
            }

            // Set the value for the argument.
            argument_ptr->value(std::string(argv[++argument_index]));
            
            // Validate the parameter value type.
            if (!ValidType(argument_ptr->valtype(), argument_ptr->value()))
            {
               // This argument requires a value, but there are no more
               // argument tokens available.  Therefore, log error and
               // continue.
               logError(argument_str, 
                        "Value specified for option does not have expected type.");
               
               argument_ptr->status(Argument::ArgBadValue);
               argument_valid = false;
            }
         }
         
         if ( argument_valid )
         {
            argument_ptr->status(Argument::ArgFound);
         }
      }
   }
   
   // Iterate through the arguments and check that all required arguments
   // were specified.
   bool all_required_params_available = true;
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
   
   return ( all_required_params_available && (errorCount() == 0));
}

//------------------------------------------------------------------------------
bool ArgParser::isSet(Argument::ArgField field, const std::string &fieldStr)
{
   Argument* argument_ptr = NULL;
   
   // if (!getArgByField(&argument_ptr, field, fieldStr))
   // {
   //    return false;
   // }
   
   // if (argument_ptr->status() != Argument::ArgFound)
   // {
   //    return false;
   // }

   bool success = false;
   if ( getArgByField( &argument_ptr, field, fieldStr ) )
   {
      if ( argument_ptr->status() == Argument::ArgFound )
      {
         success = true;
      }
   }
   
   return success;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal( int& val, Argument::ArgField field, const std::string &fieldStr)
{
   bool      success      = false;
   Argument* argument_ptr = NULL;

   if ( getArgByField( &argument_ptr, field, fieldStr ) )
   {
      if (argument_ptr->status() == Argument::ArgFound)
      {
         if ( ValidType(Argument::ArgInt, argument_ptr->value()) )
         {
            std::stringstream stream(argument_ptr->value());
   
            stream >> val;
            success = true;
         }
         else
         {
            printf("ArgParser::getArgVal(int): Arg value does not match arg type.\n");
         }
      }
      else
      {
         printf("ArgParser::getArgVal(int): Arg was not specified at the CL.\n");
      }
   }
   else
   {
      printf("ArgParser::getArgVal(int): Failed to get arg by field.\n");
   }
   
   return success;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal( float& val, Argument::ArgField field, const std::string &fieldStr)
{
   bool      success      = false;
   Argument* argument_ptr = NULL;
   
   if ( getArgByField(&argument_ptr, field, fieldStr) )
   {
      if (argument_ptr->status() == Argument::ArgFound)
      {
         if ( ValidType(Argument::ArgFloat, argument_ptr->value()) )
         {
            std::stringstream stream(argument_ptr->value());
   
            stream >> val;
            success = true;
         }
         else
         {
            printf("ArgParser::getArgVal(float): Arg value does not match arg type.\n");
         }
      }
      else
      {
         printf("ArgParser::getArgVal(float): Arg was not specified at the CL.\n");
      }
   }
   else
   {
      printf("ArgParser::getArgVal(float): Failed to get arg by field.\n");
   }
   
   return success;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal( std::string &val, Argument::ArgField field, const std::string &fieldStr )
{
   bool      success      = false;
   Argument* argument_ptr = NULL;
   
   if ( getArgByField(&argument_ptr, field, fieldStr) )
   {
      if ( argument_ptr->status() == Argument::ArgFound )
      {
         if ( ValidType( Argument::ArgString, argument_ptr->value() ) )
         {
            // Don't need a string stream since the value is already a string.
            val = argument_ptr->value();
            success = true;
         }
         else
         {
            printf("ArgParser::getArgVal(string): Arg value does not match arg type.\n");
         }
      }
      else
      {
         printf("ArgParser::getArgVal(string): Arg was not specified at the CL.\n");
      }
   }
   else
   {
      printf("ArgParser::getArgVal(string): Failed to get arg by field.\n");
   }
   
   return success;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgByField(Argument** pArg, 
                              Argument::ArgField field, 
                              const std::string &fieldStr)
{
   bool found_argument = true;
   std::map<std::string,Argument*>::const_iterator argument_iterator;
   
   switch ( field )
   {
      case Argument::ArgPrimary:
         argument_iterator = primary_table_.find( fieldStr );
         found_argument    = ( argument_iterator != primary_table_.end() );
         break;
         
      case Argument::ArgAlt:
         argument_iterator = alt_table_.find( fieldStr );
         found_argument = ( argument_iterator != primary_table_.end() );
         break;
      
      case Argument::ArgName:
         argument_iterator = name_table_.find( fieldStr );
         found_argument    = ( argument_iterator != name_table_.end() );
         break;
         
      default:
         found_argument = false;
         break;
   }

   if ( found_argument )
   {
      *pArg = argument_iterator->second;
   }
   
   return found_argument;
}

//------------------------------------------------------------------------------
void ArgParser::printArgErrors(bool bPrinHelp)
{
   std::vector<std::string>::iterator l_VIt = argument_errors_.begin();
   
   if (argument_errors_.empty())
   {
      return;
   }
   
   printf("Encountered %lu errors while parsing CLI arguments:\n", 
          argument_errors_.size());
   
   for (; l_VIt < argument_errors_.end(); ++l_VIt)
   {
      printf("   %s\n", l_VIt->c_str());
   }
   
   if (!bPrinHelp)
   {
      printf("For help, specify \"-h\" or \"--help\".\n\n");
   }
   else
   {
      printHelp();
   }
}

//------------------------------------------------------------------------------
void ArgParser::printHelp()
{
   std::vector<Argument*>::iterator argument_iterator = arguments_.begin();
   
   if ( arguments_.empty() == false )
   {
      printf("Program options:\n");
   
      for (; argument_iterator < arguments_.end(); ++argument_iterator )
      {
         Argument* argument_ptr = *argument_iterator;

         printf("  %s [\"-%s\"/\"--%s\",%s]: %s\n", 
                argument_ptr->name().c_str(),
                argument_ptr->primary().c_str(),
                argument_ptr->alt().c_str(),
                (argument_ptr->policy() == Argument::ArgRequired) ? "REQUIRED" : "OPTIONAL",
                argument_ptr->description().c_str());
      }
   }
   else
   {
      printf("Program has no registered options.\n");
   }
}

//------------------------------------------------------------------------------
void ArgParser::logError(const std::string &optId, const std::string &msg)
{
   argument_errors_.push_back(optId + " : " + msg);
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
   std::vector<Argument*>::iterator argument_iterator = arguments_.begin();
   
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
   std::stringstream l_StringStream(value);
   
   if (type == Argument::ArgInt)
   {
      int l_nTempInt = 0;
      
      l_StringStream >> l_nTempInt;
      
      return (l_StringStream.fail() == false);
   }
   else if (type == Argument::ArgFloat)
   {
      float l_nTempFloat = 0;
      
      l_StringStream >> l_nTempFloat;
      
      return (l_StringStream.fail() == false);
   }
   else if (type == Argument::ArgString)
   {
      return true;
   }
   
   return false;
}

//------------------------------------------------------------------------------
void ArgParser::printVec(const std::string &title, 
                         const std::vector<Argument*> &vec)
{
   std::vector<Argument*>::const_iterator it = vec.begin();
   
   std::cout << title << std::endl;
   
   for (; it < vec.end(); ++it)
   {
      printf("item: %lu\n", (unsigned long)(*it));
   }
}

//------------------------------------------------------------------------------
void ArgParser::printMap(const std::string &title, 
                         const std::map<std::string, Argument*> &map)
{
   std::map<std::string, Argument*>::const_iterator it = map.begin();
   
   std::cout << title << std::endl;
   
   for (; it != map.end(); ++it)
   {
      printf("k: %s, v: %lu\n", it->first.c_str(), (unsigned long)(it->second));
   }
}

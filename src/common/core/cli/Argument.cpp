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


#include "Log.h"
#include "Argument.h"
#include "StringHelper.h"

#define  ARG_NAME_INDEX     0
#define  ARG_VALUE_INDEX    1

using namespace coral;
using namespace coral::cli;
using namespace coral::helpers;

//------------------------------------------------------------------------------
Argument::Argument()
   : type_        ( Argument::ArgFlag )
   , policy_      ( Argument::ArgOptional )
   , value_type_  ( Argument::ArgString )
   , status_      ( Argument::ArgOmitted )
{
   // All string default to empty string.
}

//------------------------------------------------------------------------------
Argument* Argument::Create( const std::string &definition )
{
   Argument* argument_ptr = new Argument();
   
   if ( argument_ptr != NULL )
   {
      if ( argument_ptr->parse(definition) == false )
      {
         delete argument_ptr;
         argument_ptr = NULL;
      }
   }
   
   return argument_ptr;
}

//------------------------------------------------------------------------------
std::string Argument::value() const
{
   // If this is an optional argument and the argument was not specified at the
   // command line, return the default value.
   if ( ( policy_ == ArgOptional ) && value_.empty() )
   {
      return default_value_;
   }

   return value_;
}

//------------------------------------------------------------------------------
bool Argument::parse( const std::string& definition )
{
   bool parse_success = false;
   std::vector<std::string>            attribute_list;
   std::vector<std::string>::iterator  attribute_iterator;
   
   attribute_list = string_helper::split( definition, ',' );
   
   if ( attribute_list.size() > 0 )
   {
      parse_success = true;

      attribute_iterator = attribute_list.begin();
      for (; attribute_iterator < attribute_list.end(); ++attribute_iterator )
      {
         // Split the substring at the ':'
         std::vector<std::string> attributes_tokens =
            string_helper::split( *attribute_iterator, ':' );
                  
         if ( attributes_tokens.size() == 0 )
         {
            continue;
         }

         // Key-only attributes
         else if ( attributes_tokens.size() == 1 )
         {
            std::string attribute_key = attributes_tokens[ ARG_NAME_INDEX ];
            
            if ( setAttribute( string_helper::trim( attribute_key ) ) == false )
            {
               parse_success = false;
               break;
            }
         }

         // Key-value attributes
         else if ( attributes_tokens.size() == 2 )
         {
            std::string attribute_key   = attributes_tokens[ ARG_NAME_INDEX ];
            std::string attribute_value = attributes_tokens[ ARG_VALUE_INDEX ];
            
            if ( setAttribute( string_helper::trim( attribute_key ), 
                           string_helper::trim( attribute_value ) ) == false )
            {
               parse_success = false;
               break;
            }
         }
      }

      // Check for a couple of special cases.
      if ( type_ == ArgFlag )
      {
         // Flag arguments are always optional.
         policy_ = ArgOptional;
      }

      // Validate the minimum set of attributes.
      if ( parse_success )
      {
         parse_success &= ( name_.empty() == false );
         parse_success &= ( id_.empty() == false );
      }
   }
   
   return parse_success;
}

//------------------------------------------------------------------------------
bool Argument::setAttribute(const std::string &attribute_name, 
                            const std::string &attribute_value )
{
   bool success = true;

   if ( attribute_name.compare("name") == 0 )
   {
      if (attribute_value.empty())
      {
         success = false;
      }
      else
      {
         name_ = attribute_value;
      }
   }
   else if ( attribute_name.compare( "primary" ) == 0 )
   {
      if (attribute_value.empty())
      {
         success = false;
      }
      else
      {
         id_ = attribute_value;
      }
   }
   else if (attribute_name.compare("alt") == 0)
   {
      if ( attribute_value.empty() )
      {
         success = false;
      }
      else if ( attribute_value.compare("help") == 0 )
      {
         // "help" is reserved.
         success = false;
      }
      else
      {
         alternate_id_ = attribute_value;
      }
   }
   else if (attribute_name.compare("desc") == 0 || 
            attribute_name.compare("description") == 0)
   {
      if (attribute_value.empty())
      {
         success = false;
      }
      else
      {
         description_ = attribute_value;
      }
   }
   else if (attribute_name.compare("type") == 0)
   {
      if (attribute_value.empty())
      {
         success = false;
      }
      else
      {
         if (attribute_value.compare("flag") == 0)
         {
            type_ = ArgFlag;
         }
         else if (attribute_value.compare("opt") == 0 || 
                  attribute_value.compare("option") == 0)
         {
            type_ = ArgOption;
         }
         else
         {
            success = false;
         }
      }
   }
   else if (attribute_name.compare("required") == 0)
   {
      policy_ = ArgRequired;
   }
   else if (attribute_name.compare("def") == 0 ||
            attribute_name.compare("default") == 0)
   {
      if (attribute_value.empty())
      {
         success = false;
      }
      else
      {
         default_value_ = attribute_value;
      }
   }
   else if (attribute_name.compare("valtype") == 0 ||
            attribute_name.compare("vtype") == 0)
   {
      if (attribute_value.empty())
      {
         success = false;
      }
      else
      {
         if (attribute_value.compare("int") == 0 ||
             attribute_value.compare("integer") == 0)
         {
            value_type_ = ArgInt;
         }
         else if (attribute_value.compare("float") == 0)
         {
            value_type_ = ArgFloat;
         }
         else if (attribute_value.compare("string") == 0)
         {
            value_type_ = ArgString;
         }
         else
         {
            success = false;
         }
      }
   }
   else
   {
      log::debug(
         "Argument::setAttribute: Ignoring invalid attribute '%d'\n",
         attribute_name.c_str() );
   }
   
   return success;
}

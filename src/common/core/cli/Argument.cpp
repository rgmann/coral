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

#define  NAME     0
#define  VALUE    1

using namespace liber;
using namespace liber::cli;

//------------------------------------------------------------------------------
Argument::Argument()
{
   m_sName = "";
   m_sId = "";
   m_sAltId = "";
   m_sDescription = "";
   m_Type = ArgOption;
   m_Policy = ArgOptional;
   m_ValType = ArgString;
   
   m_sValue = "";
   m_sDefault = "";
   m_Status = ArgOmitted;
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
bool Argument::parse( const std::string& definition )
{
   bool parse_success = false;
   std::vector<std::string>            attribute_list;
   std::vector<std::string>::iterator  attribute_iterator;
   
   attribute_list = StringHelper::Split( definition, ',' );
   
   if ( attribute_list.size() > 0 )
   {
      parse_success = true;

      attribute_iterator = attribute_list.begin();
      for (; attribute_iterator < attribute_list.end(); ++attribute_iterator )
      {
         // Split the substring at the ':'
         std::vector<std::string> attributes_tokens =
            StringHelper::Split( *attribute_iterator, ':' );
                  
         if ( attributes_tokens.size() == 0 )
         {
            continue;
         }

         // Key-only attributes
         else if ( attributes_tokens.size() == 1 )
         {
            std::string attribute_key = attributes_tokens[ NAME ];
            
            if ( setAttribute( StringHelper::Trim( attribute_key ) ) == false )
            {
               parse_success = false;
               break;
            }
         }

         // Key-value attributes
         else if ( attributes_tokens.size() == 2 )
         {
            std::string attribute_key   = attributes_tokens[ NAME ];
            std::string attribute_value = attributes_tokens[ VALUE ];
            
            if ( setAttribute( StringHelper::Trim( attribute_key ), 
                           StringHelper::Trim( attribute_value ) ) == false )
            {
               parse_success = false;
               break;
            }
         }
      }

      // Validate the minimum set of attributes.
      if ( parse_success )
      {
         parse_success &= ( m_sName.empty() == false );
         parse_success &= ( m_sId.empty() == false );
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
         m_sName = attribute_value;
      }
   }
   else if (attribute_name.compare("primary") == 0)
   {
      if (attribute_value.empty())
      {
         success = false;
      }
      else
      {
         m_sId = attribute_value;
      }
   }
   else if (attribute_name.compare("alt") == 0)
   {
      if (attribute_value.empty())
      {
         success = false;
      }
      else
      {
         m_sAltId = attribute_value;
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
         m_sDescription = attribute_value;
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
            m_Type = ArgFlag;
         }
         else if (attribute_value.compare("opt") == 0 || 
                  attribute_value.compare("option") == 0)
         {
            m_Type = ArgOption;
         }
         else
         {
            success = false;
         }
      }
   }
   else if (attribute_name.compare("required") == 0)
   {
      m_Policy = ArgRequired;
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
         m_sDefault = attribute_value;
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
            m_ValType = ArgInt;
         }
         else if (attribute_value.compare("float") == 0)
         {
            m_ValType = ArgFloat;
         }
         else if (attribute_value.compare("string") == 0)
         {
            m_ValType = ArgString;
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

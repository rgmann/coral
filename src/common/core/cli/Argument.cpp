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

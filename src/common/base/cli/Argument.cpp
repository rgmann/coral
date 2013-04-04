#include "Argument.h"
#include "StringHelper.h"

#define  NAME     0
#define  VALUE    1

//------------------------------------------------------------------------------
Argument::Argument()
{
   initialize();
}

//------------------------------------------------------------------------------
Argument* Argument::Create(const std::string &definition)
{
   Argument*   l_pArgument = NULL;
   
   l_pArgument = new Argument();
   if (l_pArgument == NULL)
   {
      return NULL;
   }
   
   if (!l_pArgument->parse(definition))
   {
      delete l_pArgument;
      l_pArgument = NULL;
   }
   
   return l_pArgument;
}

//------------------------------------------------------------------------------
bool Argument::parse(const std::string &definition)
{
   bool  l_bSuccess = true;
   std::vector<std::string>            l_vAttributes;
   std::vector<std::string>::iterator  l_vAttrIt;
   
   l_vAttributes = StringHelper::Split(definition, ',');
   
   l_vAttrIt = l_vAttributes.begin();
   for (; l_vAttrIt < l_vAttributes.end(); ++l_vAttrIt)
   {
      std::vector<std::string> l_vNameVal;
      std::string              l_sName;
      std::string              l_sValue;
      
      // Split the substring at the ':'
      l_vNameVal = StringHelper::Split(*l_vAttrIt, ':');
      
      if (l_vNameVal.size() == 0)
      {
         continue;
      }
      else if (l_vNameVal.size() == 1)
      {
         l_sName = l_vNameVal[NAME];
         
         if (!setAttribute(StringHelper::Trim(l_sName)))
         {
            l_bSuccess = false;
            break;
         }
      }
      else if (l_vNameVal.size() == 2)
      {
         l_sName = l_vNameVal[NAME];
         l_sValue = l_vNameVal[VALUE];
         
         if (!setAttribute(StringHelper::Trim(l_sName), 
                      StringHelper::Trim(l_sValue)))
         {
            l_bSuccess = false;
            break;
         }
      }
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool Argument::setAttribute(const std::string &attrName, 
                            const std::string &attrVal)
{
   if (attrName.compare("name") == 0)
   {
      if (attrVal.empty())
      {
         return false;
      }
      m_sName = attrVal;
   }
   else if (attrName.compare("primary") == 0)
   {
      if (attrVal.empty())
      {
         return false;
      }
      m_sId = attrVal;
   }
   else if (attrName.compare("alt") == 0)
   {
      if (attrVal.empty())
      {
         return false;
      }
      m_sAltId = attrVal;
   }
   else if (attrName.compare("desc") == 0 || 
            attrName.compare("description") == 0)
   {
      if (attrVal.empty())
      {
         return false;
      }
      m_sDescription = attrVal;
   }
   else if (attrName.compare("type") == 0)
   {
      if (attrVal.empty())
      {
         return false;
      }
      
      if (attrVal.compare("flag") == 0)
      {
         m_Type = ArgFlag;
      }
      else if (attrVal.compare("opt") == 0 || 
               attrVal.compare("option") == 0)
      {
         m_Type = ArgOption;
      }
      else
      {
         return false;
      }
   }
   else if (attrName.compare("required") == 0)
   {
      m_Policy = ArgRequired;
   }
   else if (attrName.compare("def") == 0 ||
            attrName.compare("default") == 0)
   {
      if (attrVal.empty())
      {
         return false;
      }
      
      m_sDefault = attrVal;
   }
   else if (attrName.compare("valtype") == 0 ||
            attrName.compare("vtype") == 0)
   {
      if (attrVal.empty())
      {
         return false;
      }
      
      if (attrVal.compare("int") == 0 ||
          attrVal.compare("integer") == 0)
      {
         m_ValType = ArgInt;
      }
      else if (attrVal.compare("float") == 0)
      {
         m_ValType = ArgFloat;
      }
      else if (attrVal.compare("string") == 0)
      {
         m_ValType = ArgString;
      }
      else
      {
         return false;
      }
   }
   
   return true;
}

//------------------------------------------------------------------------------
void  Argument::initialize()
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

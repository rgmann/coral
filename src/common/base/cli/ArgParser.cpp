#include "ArgParser.h"
#include <iostream>
#include <sstream>
#include <stdio.h>

//------------------------------------------------------------------------------
ArgParser::ArgParser()
{
}

//------------------------------------------------------------------------------
ArgParser::~ArgParser()
{
   std::vector<Argument*>::iterator it = m_ArgVector.begin();
   
   m_PrimaryMap.clear();
   m_AltMap.clear();
   m_NameMap.clear();
   
   for (; it < m_ArgVector.end(); ++it)
   {
      if (*it != NULL)
      {
         delete *it;
         *it = NULL;
      }
   }
   
   m_ArgVector.clear();
}

//------------------------------------------------------------------------------
bool ArgParser::addArg(const std::string &arg)
{
   Argument* l_pArg = NULL;
   
   l_pArg = Argument::Create(arg);
   
   if (!l_pArg)
   {
      return false;
   }
   
   m_ArgVector.push_back(l_pArg);
   m_PrimaryMap[l_pArg->primary()] = l_pArg;
   
   if (!l_pArg->alt().empty())
   {
      m_AltMap[l_pArg->alt()] = l_pArg;
   }
   
   if (!l_pArg->name().empty())
   {
      m_NameMap[l_pArg->name()] = l_pArg;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool ArgParser::parse(const char* argv[], int argc)
{
   std::string l_sArgStr = "";
   std::string l_sArgId = "";
   std::string l_sArgVal = "";
   Argument*   l_pArg = NULL;
   
   bool        l_bAllReqParamsAvail = true;
   std::vector<Argument*>::iterator  l_VIt;
   
   // Reset the error log
   resetErrorLog();
   
   // Reset the arguments
   resetArgs();
   
   // Start at one to skip over program name
   for (int l_nArg = 1; l_nArg < argc; l_nArg++)
   {
      bool l_bMoreArgsAvail = ((l_nArg + 1) < argc);
      l_sArgStr = std::string(argv[l_nArg]);
      
      // Search secondary keys if the arg starts with "--"
      if (l_sArgStr.find("--") == 0)
      {
         l_sArgId = l_sArgStr.substr(2);
         
         //if (m_SecondaryMap.count(l_sArgId) > 0)
         if (m_AltMap.count(l_sArgId) > 0)
         {
            l_pArg = m_AltMap[l_sArgId];
         }
         else
         {
            // Unrecognized arg.  Log error and continue.
            logError(l_sArgStr, "Unrecognized option.");
            continue;
         }
      }
      else if (l_sArgStr.find("-") == 0)
      {
         l_sArgId = l_sArgStr.substr(1);
         
         if (m_PrimaryMap.count(l_sArgId) > 0)
         {
            l_pArg = m_PrimaryMap[l_sArgId];
         }
         else
         {
            // Unrecognized arg.  Log error and continue.
            logError(l_sArgStr, "Unrecognized option.");
            continue;
         }
      }
      else
      {
         // All arg IDs should start with "-" or "--".
         // Therefore log an error.
         logError(l_sArgStr, "Option flags must contain \"-\" or\"--\".");
         continue;
      }
      
      // Currently all option type args require a value to be specified.
      if (l_pArg->type() == Argument::ArgOption)
      {
         if (!l_bMoreArgsAvail)
         {
            // This argument requires a value, but there are no more
            // argument tokens available.  Therefore, log error and
            // continue.
            logError(l_sArgStr, "No value provided for option.");
            
            l_pArg->status(Argument::ArgMissingParam);
            continue;
         }

         // Set the value for the argument.
         l_pArg->value(std::string(argv[++l_nArg]));
         
         // Validate the parameter value type.
         if (!ValidType(l_pArg->valtype(), l_pArg->value()))
         {
            // This argument requires a value, but there are no more
            // argument tokens available.  Therefore, log error and
            // continue.
            logError(l_sArgStr, 
                     "Value specified for option does not have expected type.");
            
            l_pArg->status(Argument::ArgBadValue);
            continue;
         }
      }
      
      l_pArg->status(Argument::ArgFound);
   }
   
   // Iterate through the arguments and check that all required arguments
   // were specified.
   l_VIt = m_ArgVector.begin();
   for (; l_VIt < m_ArgVector.end(); ++l_VIt)
   {
      if ((*l_VIt)->policy() == Argument::ArgRequired &&
          (*l_VIt)->status() != Argument::ArgFound)
      {
         if (l_bAllReqParamsAvail)
         {
            l_bAllReqParamsAvail = false;
         }
         
         logError((*l_VIt)->primary(), "Required parameter missing.");
      }
   }
   
   return (l_bAllReqParamsAvail && (errorCount() == 0));
}

//------------------------------------------------------------------------------
bool ArgParser::isSet(Argument::ArgField field, const std::string &fieldStr)
{
   Argument* l_pArg = NULL;
   
   if (!getArgByField(&l_pArg, field, fieldStr))
   {
      return false;
   }
   
   if (l_pArg->status() != Argument::ArgFound)
   {
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal(int &val, Argument::ArgField field, const std::string &fieldStr)
{
   Argument* l_pArg = NULL;
   
   if (!getArgByField(&l_pArg, field, fieldStr))
   {
      printf("ArgParser::getArgVal(int): Failed to get arg by field.\n");
      return false;
   }
   
   if (l_pArg->status() != Argument::ArgFound)
   {
      printf("ArgParser::getArgVal(int): Arg was not specified at the CL.\n");
      return false;
   }
   
   if (!ValidType(Argument::ArgInt, l_pArg->value()))
   {
      printf("ArgParser::getArgVal(int): Arg value does not match arg type.\n");
      return false;
   }
   
   std::stringstream l_StringStream(l_pArg->value());
   
   l_StringStream >> val;
   
   return true;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal(float &val, Argument::ArgField field, const std::string &fieldStr)
{
   Argument* l_pArg = NULL;
   
   if (!getArgByField(&l_pArg, field, fieldStr))
   {
      printf("ArgParser::getArgVal(float): Failed to get arg by field.\n");
      return false;
   }
   
   if (l_pArg->status() != Argument::ArgFound)
   {
      printf("ArgParser::getArgVal(float): Arg was not specified at the CL.\n");
      return false;
   }
   
   if (!ValidType(Argument::ArgFloat, l_pArg->value()))
   {
      printf("ArgParser::getArgVal(float): Arg value does not match arg type.\n");
      return false;
   }
   
   std::stringstream l_StringStream(l_pArg->value());
   
   l_StringStream >> val;
   
   return true;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgVal(std::string &val, Argument::ArgField field, const std::string &fieldStr)
{
   Argument* l_pArg = NULL;
   
   if (!getArgByField(&l_pArg, field, fieldStr))
   {
      printf("ArgParser::getArgVal(string): Failed to get arg by field.\n");
      return false;
   }
   
   if (l_pArg->status() != Argument::ArgFound)
   {
      printf("ArgParser::getArgVal(string): Arg was not specified at the CL.\n");
      return false;
   }
   
   if (!ValidType(Argument::ArgString, l_pArg->value()))
   {
      printf("ArgParser::getArgVal(string): Arg value does not match arg type.\n");
      return false;
   }
   
   // Don't need a string stream since the value is already a string.
   val = l_pArg->value();
   
   return true;
}

//------------------------------------------------------------------------------
bool ArgParser::getArgByField(Argument** pArg, 
                              Argument::ArgField field, 
                              const std::string &fieldStr)
{
   bool l_bFoundArg = false;
   
   switch (field) {
      case Argument::ArgPrimary:
         if (m_PrimaryMap.count(fieldStr) > 0)
         {
            *pArg = m_PrimaryMap[fieldStr];
            l_bFoundArg = true;
         }
         break;
         
      case Argument::ArgAlt:
         if (m_AltMap.count(fieldStr) > 0)
         {
            *pArg = m_AltMap[fieldStr];
            l_bFoundArg = true;
         }
         break;
      
      case Argument::ArgName:
         if (m_NameMap.count(fieldStr) > 0)
         {
            *pArg = m_NameMap[fieldStr];
            l_bFoundArg = true;
         }
         break;
         
      default:
         break;
   }
   
   if (*pArg == NULL)
   {
      printf("ArgParser::getArgByField: NULL pointer\n");
      return false;
   }
   
   return l_bFoundArg;
}

//------------------------------------------------------------------------------
void ArgParser::printArgErrors(bool bPrinHelp)
{
   std::vector<std::string>::iterator l_VIt = m_ArgErrors.begin();
   
   if (m_ArgErrors.empty())
   {
      return;
   }
   
   printf("Encountered %lu errors while parsing CLI arguments:\n", 
          m_ArgErrors.size());
   
   for (; l_VIt < m_ArgErrors.end(); ++l_VIt)
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
   std::vector<Argument*>::iterator l_VIt = m_ArgVector.begin();
   
   if (m_ArgVector.empty())
   {
      printf("Program has no registered options.\n");
      return;
   }
   
   printf("Program options:\n");
   
   for (; l_VIt < m_ArgVector.end(); ++l_VIt)
   {
      printf("  %s [\"-%s\"/\"--%s\",%s]: %s\n", 
             (*l_VIt)->name().c_str(),
             (*l_VIt)->primary().c_str(),
             (*l_VIt)->alt().c_str(),
             ((*l_VIt)->policy() == Argument::ArgRequired) ? "REQUIRED" : "OPTIONAL",
             (*l_VIt)->description().c_str());
   }
}

//------------------------------------------------------------------------------
void ArgParser::logError(const std::string &optId, const std::string &msg)
{
   m_ArgErrors.push_back(optId + " : " + msg);
}

//------------------------------------------------------------------------------
void ArgParser::resetErrorLog()
{
   m_ArgErrors.clear();
}

//------------------------------------------------------------------------------
int ArgParser::errorCount()
{
   return m_ArgErrors.size();
}

//------------------------------------------------------------------------------
void ArgParser::resetArgs()
{
   std::vector<Argument*>::iterator l_VIt = m_ArgVector.begin();
   
   for (; l_VIt < m_ArgVector.end(); ++l_VIt)
   {
      (*l_VIt)->clearValue();
      (*l_VIt)->status(Argument::ArgOmitted);
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

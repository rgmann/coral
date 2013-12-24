#include <errno.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include "JsonObject.h"

//#define JSON_ISSPACE std::bind(std::isspace<char>, _1, std::locale::classic())
#define JSON_ISSPACE ::isspace

//-----------------------------------------------------------------------------
JsonObject::JsonObject()
{
}

//-----------------------------------------------------------------------------
JsonObject::JsonObject(const JsonObject &object)
{
   fromJsonString(object.toJsonString());
}

//-----------------------------------------------------------------------------
JsonObject::JsonObject(const std::string &object, JsonFormat format)
{
   parse(object, format);
}

//-----------------------------------------------------------------------------
JsonObject::JsonObject(const ui8* pData, ui32 nLength, JsonFormat format)
{
   parse(pData, nLength, format);
}

//-----------------------------------------------------------------------------
JsonObject::~JsonObject()
{
}

//-----------------------------------------------------------------------------
bool JsonObject::getElement(const std::string &name, JsonElement &element) const
{
   bool lbSuccess = hasField(name);
   
   if (lbSuccess)
   {
      element = mFieldMap.find(name)->second;
   }
   
   return lbSuccess;
}

//-----------------------------------------------------------------------------
void JsonObject::getElements(std::vector<JsonElement> &elements) const
{
   std::map<std::string, JsonElement>::const_iterator lElIt;
   
   lElIt = mFieldMap.begin();
   for (; lElIt != mFieldMap.end(); lElIt++)
   {
      elements.push_back(lElIt->second);
   }
}

//-----------------------------------------------------------------------------
void JsonObject::setElement(const JsonElement &element)
{
   mFieldMap[element.getName()] = element;
}

//-----------------------------------------------------------------------------
bool JsonObject::hasField(const std::string &name) const
{
   return (mFieldMap.count(name) == 1);
}

//-----------------------------------------------------------------------------
bool JsonObject::toJsonNode(JSONNode &node) const
{
   JSONNode lNode(JSON_NODE);

   //std::map<std::string, std::string>::const_iterator lObjIt;
   std::map<std::string, JsonElement>::const_iterator lObjIt;
   for (lObjIt = mFieldMap.begin(); lObjIt != mFieldMap.end(); lObjIt++)
   {
      JSONNode lElementNode;
      lObjIt->second.toJsonNode(lElementNode);
      lNode.push_back(lElementNode);
   }

   node = lNode;

   return true;
}

//-----------------------------------------------------------------------------
std::string JsonObject::toJsonString() const
{
   JSONNode lNode;
   std::string lNodeString;

   if (toJsonNode(lNode))
   {
      lNodeString = lNode.as_string();
   }

   return lNodeString;
}

//-----------------------------------------------------------------------------
bool JsonObject::fromJsonString(const std::string &object)
{
   JSONNode lNode = libjson::parse(object);
   
   JSONNode::const_iterator lNodeIt = lNode.begin();
   for (; lNodeIt != lNode.end(); lNodeIt++)
   {
      JsonElement lElement(*lNodeIt);
      
      if (!lElement.getName().empty() &&
          (lElement.getType() != JsonElement::Invalid))
      {
         mFieldMap[lElement.getName()] = lElement;
      }
   }
}

//-----------------------------------------------------------------------------
ui32 JsonObject::size() const
{
   return toJsonString().size();
}

//-----------------------------------------------------------------------------
JsonObject& JsonObject::operator << (const JsonElement &element)
{
   setElement(element);
   return *this;
}

//-----------------------------------------------------------------------------
std::string JsonObject::getString() const
{
   JSONNode lNode;
   toJsonNode(lNode);
   return lNode.write_formatted();
}

//-----------------------------------------------------------------------------
std::string JsonObject::getCompactString() const
{
   std::string lCompactStr = getString();
   lCompactStr.erase(std::remove_if(lCompactStr.begin(), 
                                    lCompactStr.end(),
                                    JSON_ISSPACE), 
                                    lCompactStr.end());
   return lCompactStr;
}

//-----------------------------------------------------------------------------
std::string JsonObject::encode() const
{
   std::string lCompactStr = getCompactString();
   return libjson::encode64((ui8*)lCompactStr.data(), lCompactStr.size());
}

//-----------------------------------------------------------------------------
void JsonObject::encode(ui8** pData, ui32 &nLength) const
{
   std::string lCompactStr = getCompactString();
   *pData = new ui8[lCompactStr.size()];
   memcpy(pData, lCompactStr.data(), lCompactStr.size());
}

//-----------------------------------------------------------------------------
bool JsonObject::parse(const ui8* pData, ui32 nLength, JsonFormat format)
{
   return parse(std::string((char*)pData, nLength), format);
}

//-----------------------------------------------------------------------------
bool JsonObject::parse(const std::string &object, JsonFormat format)
{
   std::string lObject;
   
   if (format == JsonBase64)
   {
      lObject = libjson::decode64(object);
   }
   else {
      lObject = object;
   }

   fromJsonString(lObject);
}

//-----------------------------------------------------------------------------
void JsonObject::printFormatted() const
{
   std::cout << getString() << std::endl;
}

//-----------------------------------------------------------------------------
void JsonObject::dumpBinary() const
{
   JSONNode lNode;
   toJsonNode(lNode);
      
   switch (lNode.type()) {
      case JSON_NULL:
         std::cout << "type = " << lNode.type() << " = JSON_NULL" << std::endl;
         break;
      case JSON_STRING:
         std::cout << "type = " << lNode.type() << " = JSON_STRING" << std::endl;
         break;
      case JSON_NUMBER:
         std::cout << "type = " << lNode.type() << " = JSON_NUMBER" << std::endl;
         break;
      case JSON_BOOL:
         std::cout << "type = " << lNode.type() << " = JSON_BOOL" << std::endl;
         break;
      case JSON_ARRAY:
         std::cout << "type = " << lNode.type() << " = JSON_ARRAY" << std::endl;
         break;
      case JSON_NODE:
         std::cout << "type = " << lNode.type() << " = JSON_NODE" << std::endl;
         break;
      default:
         std::cout << "type = " << lNode.type() << " = UNKNOWN" << std::endl;
         break;
   }
   
   //lNode.cast(JSON_STRING);
   for (JSONNode::const_iterator lNodeIt = lNode.begin(); 
        lNodeIt != lNode.end(); lNodeIt++) {
      switch (lNodeIt->type()) {
         case JSON_NULL:
            std::cout << " [" << lNodeIt->name() << ", JSON_NULL]" << std::endl;
            break;
         case JSON_STRING:
            std::cout << " [" << lNodeIt->name() << ", JSON_STRING] = " 
            << lNodeIt->as_string() 
            << std::endl;
            break;
         case JSON_NUMBER:
            std::cout << " [" << lNodeIt->name() << ", JSON_NUMBER]" << std::endl;
            break;
         case JSON_BOOL:
            std::cout << " [" << lNodeIt->name() << ", JSON_BOOL]" << std::endl;
            break;
         case JSON_ARRAY:
            std::cout << " [" << lNodeIt->name() << ", JSON_ARRAY]" << std::endl;
            break;
         case JSON_NODE:
            std::cout << " [" << lNodeIt->name() << ", JSON_NODE]" << std::endl;
            break;
         default:
            std::cout << " [" << lNodeIt->name() << ", UNKNOWN]" << std::endl;
            break;
      }
   }
   
   std::string lJsonString = lNode.write_formatted();
   lJsonString.erase(std::remove_if(lJsonString.begin(), 
                                    lJsonString.end(),
                                    isspace), lJsonString.end());
   std::cout << "Compact string = " << lJsonString << std::endl;

   /*JSONNode lBinaryNode;
   lBinaryNode.set_binary((ui8*)lJsonString.data(), lJsonString.size());
   std::cout << "Binary node " << std::endl;
   switch (lBinaryNode.type()) {
      case JSON_NULL:
         std::cout << "type = JSON_NULL" << std::endl;
         break;
      case JSON_STRING:
         std::cout << "type = JSON_STRING" << std::endl;
         break;
      case JSON_NUMBER:
         std::cout << "type = JSON_NUMBER" << std::endl;
         break;
      case JSON_BOOL:
         std::cout << "type = JSON_BOOL" << std::endl;
         break;
      case JSON_ARRAY:
         std::cout << "type = JSON_ARRAY" << std::endl;
         break;
      case JSON_NODE:
         std::cout << "type = JSON_NODE" << std::endl;
         break;
      default:
         std::cout << "type = UNKNOWN" << std::endl;
         break;
   }
   std::string lBinaryData = lBinaryNode.as_binary();
   std::string lStringData = lBinaryNode.as_string();
   std::cout << "Binary Dump = " << lBinaryData << std::endl;
   std::cout << "STring Dump = " << lStringData << std::endl;*/
   
   /*JSONNode lDecryptNode(JSON_STRING);
   lDecryptNode.set_binary((ui8*)lStringData.data(), lStringData.size());
   std::string decryptedData = lDecryptNode.as_string();
   std::cout << "Decrytped string = " << decryptedData << std::endl;*/
   std::string encoded = libjson::encode64((ui8*)lJsonString.data(), lJsonString.size());
   std::cout << "encoded: " << encoded << std::endl;
   std::string decoded = libjson::decode64(encoded);

   //JSONNode lReconNode = libjson::parse(lJsonString.c_str());
   JSONNode lReconNode = libjson::parse(decoded);
   std::cout << lReconNode.write_formatted() << std::endl;
   
   //std::string lBinaryData = lNode.as_binary();
   //std::cout << "Binary Dump: size = " << lBinaryData.size() << std::endl;
   //for (int lnPos = 0; lnPos < lBinaryData.size(); lnPos++) {
//      std::cout << lBinaryData[lnPos];
//   }
//   std::cout << std::endl;
}

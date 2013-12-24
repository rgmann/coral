#include <algorithm>
#include <functional>
#include <iostream>
#include "JsonTransportObject.h"

#define JSON_ISSPACE ::isspace

#define FIELD(stype, etype, field) \
   static_cast<const TField<stype, etype>*>(field)

//------------------------------------------------------------------------------
JsonTransportObject::JsonTransportObject()
:  TransportObject(TransportObject::JSON),
   mRootNode(JSON_NULL)
{
}

//------------------------------------------------------------------------------
JsonTransportObject::JsonTransportObject(const Structure &structure)
:  TransportObject(TransportObject::JSON),
   mRootNode(JSON_NULL)
{
   from(structure);
}

//------------------------------------------------------------------------------
JsonTransportObject::~JsonTransportObject()
{
}

//------------------------------------------------------------------------------
void JsonTransportObject::from(const Structure &structure)
{
   mRootNode = JSONNode(JSON_NODE);
   from(structure, mRootNode);
   //mRootNode.push_back(JSONNode("T", Field::Struct));
   //mRootNode.push_back(JSONNode("N", structure.getName()));
   
   /*JSONNode lChildNode(JSON_NODE);
   from(structure, lChildNode);
   lChildNode.set_name("V");
   mRootNode.push_back(lChildNode);*/
}

//------------------------------------------------------------------------------
void JsonTransportObject::from(const Structure &structure, JSONNode &node)
{
   std::vector<const Field*> lFields;
   std::vector<const Field*>::const_iterator lField;
   
   if (structure.getName() != "")
   {
      node.set_name(structure.getName());
   }
   
   structure.getFieldList(lFields);
   lField = lFields.begin();
   for (; lField != lFields.end(); lField++)
   {
      JSONNode lChildNode(JSON_NODE);
      toNode(*lField, lChildNode);
      node.push_back(lChildNode);
   }
}

//-----------------------------------------------------------------------------
void JsonTransportObject::toNode(const Field* pField, JSONNode &node)
{
   //node.set_name(pField->getName());
   node.push_back(JSONNode("N", pField->getName()));
   node.push_back(JSONNode("T", pField->getType()));
   
   switch (pField->getType())
   {
      case Field::Int32:
         node.push_back(JSONNode("V", FIELD(i32, Field::Int32,
                                            pField)->getValue()));
         break;
      case Field::Int64:
         node.push_back(JSONNode("V",
                                 FIELD(i64, Field::Int64,
                                       pField)->getValue()));
         break;
      case Field::Bool:
         node.push_back(JSONNode("V",
                                 FIELD(bool, Field::Bool,
                                       pField)->getValue()));
         break;
      case Field::Double:
         node.push_back(JSONNode("V",
                                 FIELD(double, Field::Double,
                                       pField)->getValue()));
         break;
      case Field::String:
         node.push_back(JSONNode("V",
                                 FIELD(std::string, Field::String,
                                       pField)->getValue()));
         break;
      case Field::Struct:
      {
         JSONNode lChildNode(JSON_NODE);
         from(FIELD(Structure, Field::Struct, pField)->getValue(),
              lChildNode);
         lChildNode.set_name("V");
         node.push_back(lChildNode);
      }
         break;
      default:
         break;
   }
}

//-----------------------------------------------------------------------------
void JsonTransportObject::to(Structure &structure) const
{
   to(mRootNode, structure);
}

//-----------------------------------------------------------------------------
void JsonTransportObject::to(const JSONNode &node, Structure &structure) const
{
   if (node.type() == JSON_NODE)
   {
      JSONNode::const_iterator lNodeIt = node.begin();
      for (; lNodeIt != node.end(); ++lNodeIt)
      {
         const JSONNode& rNode = *lNodeIt;
         std::string lsNodeName = rNode["N"].as_string();
         Field::Type lType = (Field::Type)rNode["T"].as_int();
         switch (lType)
         {
            case Field::Int32: structure.set(lsNodeName, (i32)rNode["V"].as_int()); break;
            case Field::Int64: structure.set(lsNodeName, (i64)rNode["V"].as_int()); break;
            case Field::Bool: structure.set(lsNodeName, (bool)rNode["V"].as_bool()); break;
            case Field::Double: structure.set(lsNodeName, (double)rNode["V"].as_float()); break;
            case Field::String: structure.set(lsNodeName, (std::string)rNode["V"].as_string()); break;
            case Field::Struct:
            {
               Structure lNodeStruct;
               to(rNode["V"].as_node(), lNodeStruct);
               structure.set(lsNodeName, lNodeStruct);
            }
               break;
            default:
               break;
         }
      }      
   }
}

//-----------------------------------------------------------------------------
std::string JsonTransportObject::getString() const
{
   return mRootNode.write_formatted();
}

//-----------------------------------------------------------------------------
bool JsonTransportObject::getData(TransportObject::Encoding encoding,
                                  ui8** pData, 
                                  ui32 &nSizeBytes) const
{
   const ui8* lpData = NULL;
   std::string lEncodedStr;
   std::string lString = getCompactString();
   nSizeBytes = 0;
      
   if (*pData != NULL)
   {
      return false;
   }
   
   if (encoding == TransportObject::Text)
   {
      lpData = reinterpret_cast<const ui8*>(lString.data());
      nSizeBytes = lString.size();
   }
   else if (encoding == TransportObject::Base64)
   {
      lEncodedStr = libjson::encode64((ui8*)lString.data(), lString.size());
      nSizeBytes = lEncodedStr.size();
      lpData = reinterpret_cast<const ui8*>(lEncodedStr.data());
   }
   
   //std::cout << "JTO::getData: allocated " << nSizeBytes << " bytes."
   //<< std::endl;
   
   if (lpData && nSizeBytes > 0)
   {
      *pData = new ui8[nSizeBytes];
      memcpy(*pData, lpData, nSizeBytes);
   }
   
   return (pData != NULL);
}

//-----------------------------------------------------------------------------
std::string JsonTransportObject::getCompactString() const
{
   std::string lCompactStr = getString();
   lCompactStr.erase(std::remove_if(lCompactStr.begin(), 
                                    lCompactStr.end(),
                                    JSON_ISSPACE), 
                     lCompactStr.end());
   return lCompactStr;
}

//-----------------------------------------------------------------------------
void JsonTransportObject::setData(TransportObject::Encoding encoding,
                                  const ui8* pData, 
                                  ui32 nSizeBytes)
{
   parse(encoding, std::string((char*)pData, nSizeBytes));
}

//-----------------------------------------------------------------------------
void JsonTransportObject::parse(TransportObject::Encoding encoding,
                                const std::string &object)
{
   std::string lObject;
   
   if (encoding == TransportObject::Base64)
   {
      lObject = libjson::decode64(object);
   }
   else
   {
      lObject = object;
   }
   
   fromJsonString(lObject);
}

//-----------------------------------------------------------------------------
void JsonTransportObject::fromJsonString(const std::string &object)
{
   mRootNode = libjson::parse(object);
}

//-----------------------------------------------------------------------------
void JsonTransportObject::printFormatted() const
{
   std::cout << getString() << std::endl;
}

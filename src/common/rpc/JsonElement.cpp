#include <errno.h>
#include <sstream>
//#include "libjson.h"
#include "JsonObject.h"


//-----------------------------------------------------------------------------
JsonElement::JsonElement()
: mType(JsonElement::Invalid)
{
}

//-----------------------------------------------------------------------------
JsonElement::JsonElement(const JsonElement &other)
{
   *this = other;
}

//-----------------------------------------------------------------------------
JsonElement::JsonElement(const std::string &name, const std::string &value)
: mType(JsonElement::String),
  mNode(name, value)
{
}

//-----------------------------------------------------------------------------
JsonElement::JsonElement(const std::string &name, int value)
: mType(JsonElement::Number),
  mNode(name, value)
{
}

//-----------------------------------------------------------------------------
JsonElement::JsonElement(const std::string &name, float value)
: mType(JsonElement::Number),
  mNode(name, value)
{
}

//-----------------------------------------------------------------------------
JsonElement::JsonElement(const std::string &name, bool value)
: mType(JsonElement::Bool),
  mNode(name, value)
{
}

//-----------------------------------------------------------------------------
JsonElement::JsonElement(const std::string &name, const JsonObject &value)
: mType(JsonElement::Object),
  mNode(JSON_NODE)
{
   std::vector<JsonElement> lElVec;
   std::vector<JsonElement>::iterator lElIt;
   
   value.getElements(lElVec);
   lElIt = lElVec.begin();
   for (; lElIt != lElVec.end(); lElIt++)
   {
      mNode.push_back(lElIt->mNode);
   }
   
   mNode.set_name(name);
}

//-----------------------------------------------------------------------------
JsonElement::JsonElement(const JSONNode &value)
: mNode(value)
{
   switch (value.type())
   {
      case JSON_STRING:
         mType = String;
         break;
      case JSON_NUMBER:
         mType = Number;
         break;
      case JSON_BOOL:
         mType = Bool;
         break;
      case JSON_ARRAY:
         mType = Array;
         break;
      case JSON_NODE:
         mType = Object;
         break;
      case JSON_NULL:
      default:
         std::cout << "JsonElement::type = UNKNOWN" << std::endl;
         break;
   }
}

//-----------------------------------------------------------------------------
JsonElement::Type JsonElement::getType() const
{
   return mType;
}

//-----------------------------------------------------------------------------
std::string JsonElement::getName() const
{
   return mNode.name();
}

//-----------------------------------------------------------------------------
bool JsonElement::getValue(std::string &value) const
{
   bool lbCorrectType = (mType == String);
   if (lbCorrectType) value = mNode.as_string();
   return lbCorrectType;
}

//-----------------------------------------------------------------------------
bool JsonElement::getValue(int &value) const
{
   bool lbCorrectType = (mType == Number);
   if (lbCorrectType) value = mNode.as_int();
   return lbCorrectType;
}

//-----------------------------------------------------------------------------
bool JsonElement::getValue(float &value) const
{
   bool lbCorrectType = (mType == Number);
   if (lbCorrectType) value = mNode.as_float();
   return lbCorrectType;
}

//-----------------------------------------------------------------------------
bool JsonElement::getValue(bool &value) const
{
   bool lbCorrectType = (mType == Bool);
   if (lbCorrectType) value = mNode.as_bool();
   return lbCorrectType;
}

//-----------------------------------------------------------------------------
bool JsonElement::getValue(JsonObject &value) const
{
   bool lbCorrectType = (mType == Object);
   if (lbCorrectType) value = JsonObject(mNode.as_binary());
   return lbCorrectType;
}

//-----------------------------------------------------------------------------
bool JsonElement::isValid() const
{
   return (mType != Invalid);
}

//-----------------------------------------------------------------------------
JsonElement& JsonElement::operator = (const JsonElement &other)
{
   if (this == &other) return *this;

   mNode = other.mNode;
   mType = other.mType;

   return *this;
}

//-----------------------------------------------------------------------------
bool JsonElement::toJsonNode(JSONNode &node) const
{
   bool lbValid = (mType != Invalid);
   
   if (lbValid)
   {
      node = mNode;
   }
   
   return lbValid;
}

#ifndef JSON_ELEMENT_H
#define JSON_ELEMENT_H

#include <map>
#include <string>
#include "libjson.h"
#include "BaseTypes.h"

class JsonObject;

class JsonElement
{
public:

   enum Type
   {
      Invalid,
      String,
      Number,
      Bool,
      Array,
      Object
   };

   JsonElement();
   JsonElement(const JsonElement &);
   JsonElement(const std::string &name, const std::string &value);
   JsonElement(const std::string &name, int   value);
   JsonElement(const std::string &name, float value);
   JsonElement(const std::string &name, bool  value);
   JsonElement(const std::string &name, const JsonObject &value);
   JsonElement(const JSONNode &node);

   Type getType() const;
   std::string getName() const;

   bool getValue(std::string &value) const;
   bool getValue(int &value) const;
   bool getValue(float &value) const;
   bool getValue(bool  &value) const;
   bool getValue(JsonObject &value) const;

   bool isValid() const;

   JsonElement& operator = (const JsonElement &);
   
   bool toJsonNode(JSONNode &node) const;

private:

   Type mType;

   JSONNode mNode;
};

#endif // JSON_ELEMENT_H

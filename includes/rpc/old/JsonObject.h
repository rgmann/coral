#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

#include <map>
#include <vector>
#include <string>
#include "libjson.h"
#include "JsonElement.h"

class JsonObject
{
public:

   enum JsonFormat
   {
      JsonText,
      JsonBase64
   };
   
   JsonObject();
   JsonObject(const JsonObject &object);
   JsonObject(const std::string &object, JsonFormat format = JsonText);
   JsonObject(const ui8* pData, ui32 nLength, JsonFormat format = JsonText);

   virtual ~JsonObject();

   bool getElement(const std::string &name, JsonElement &value) const;
   void getElements(std::vector<JsonElement> &elements) const;

   JsonObject& operator << (const JsonElement &element);
   void setElement(const JsonElement &value);

   bool hasField(const std::string &name) const;

   std::string toJsonString() const;
   bool fromJsonString(const std::string &object);
   
   ui32 size() const;

   std::string getString() const;
   std::string getCompactString() const;
   
   std::string encode() const;
   void encode(ui8** pData, ui32 &nLength) const;
   
   
   bool parse(const std::string &encodedObject, JsonFormat format = JsonText);
   bool parse(const ui8* pData, const ui32 nLength, JsonFormat format = JsonText);
   
   void printFormatted() const;
   
   void dumpBinary() const;

private:

   bool toJsonNode(JSONNode &node) const;

private:

   std::map<std::string, JsonElement> mFieldMap;
};

#endif // JSON_OBJECT_H

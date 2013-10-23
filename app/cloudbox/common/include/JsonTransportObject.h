#ifndef JSON_TRANSPORT_OBJECT_H
#define JSON_TRANSPORT_OBJECT_H

#include "libjson.h"
#include "Structure.h"

class JsonTransportObject : public TransportObject
{
public:
   
   enum JsonFormat
   {
      JsonText,
      JsonBase64
   };
   
   JsonTransportObject();
   JsonTransportObject(const Structure &structure);
   ~JsonTransportObject();
   
   void from(const Structure &structure);
   void to(Structure &structure) const;
   
   std::string getString() const;
   std::string getCompactString() const;
   
   bool getData(TransportObject::Encoding encoding,
                ui8** pData, 
                ui32 &nSizeBytes) const;
   
   void setData(TransportObject::Encoding encoding,
                const ui8* pData,
                ui32 nSizeBytes);
   
   void parse(TransportObject::Encoding encoding,
              const ui8* pData,
              ui32 nLength);
   void parse(TransportObject::Encoding encoding,
              const std::string &object);
   
   void fromJsonString(const std::string &object);
   
   void printFormatted() const;
   
   std::string toString() const
   {
      return getString();
   };
   
private:
   
   void from(const Structure &structure, JSONNode &node);
   
   void toNode(const Field* pField, JSONNode &node);
   
   void to(const JSONNode &node, Structure &structure) const;
   
private:
   
   JSONNode mRootNode;
};

#endif // JSON_TRANSPORT_OBJECT_H
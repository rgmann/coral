#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "libjson.h"
#include <string>

class Serializable {
public:
   
   virtual std::string  toJson() = 0;
   
   virtual bool         fromJson(const std::string &jsonString) = 0;
   
protected:
   
   virtual void         parseNode(const JSONNode &n) = 0;
};

#endif // SERIALIZABLE_H
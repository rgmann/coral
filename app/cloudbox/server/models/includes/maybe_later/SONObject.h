#ifndef SON_OBJECT_H
#define SON_OBJECT_H

#include <string>
#include "mongo/client/dbclient.h"

/**
 * Script Object Notiation Object
 */
class SONObject
{
public:
   
   SONObject();
   
   ~SONObject();
   
   void  collection(const std::string &name);
   std::string collection();
   
   BSONObj& bsonObj();
   
   BSONObj toBsonObj();
   
private:
   
   std::string myCollectionName;
   
   std::map<
   
   BSONObj  myBsonObj;
};

#endif // SON_OBJECT_H

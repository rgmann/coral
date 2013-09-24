#ifndef GENERIC_MODEL_H
#define GENERIC_MODEL_H

#include <string>
#include <vector>
#include "BaseTypes.h"
#include "MongoConnection.h"


/**
 * Abstract Model base class to abstract DB model interface.
 *
 *
 *
 */

typedef mongo::OID ObjectId;

class GenericModel
{
public:
   
   /**
    * A model created without a connection can only be used for querying.
    */
   GenericModel(const std::string &modelName);
         
   virtual ~GenericModel();
            
   std::string idToString() const;
   
   std::string modelName();
   
   std::string collectionName();
   
   /**
    * Indicates whether the 
    */
   bool isValid() const;
      
   const mongo::BSONObj&   object() const;
   mongo::BSONObj&         object();
   
   void object(const mongo::BSONObj &obj);
   
   bool getObjectId(ObjectId &objectId) const;
   
   /**
    * Resets the BSONObj.  The base class simply resets to an empty object,
    * but derived models may override so that clear resets fields while keeping
    * field names.
    */
   virtual void clear();
   
protected:
   
   std::string         m_sModelName;

   mongo::BSONObj      m_bsonObject;
   
};

template <class A>
bool convert(std::vector<A*> &to, const std::vector<GenericModel*> &from)
{
   bool l_bSuccess = true;
   
   std::vector<GenericModel*>::const_iterator fromIt = from.begin();
   for (; fromIt != from.end(); fromIt++)
   {
      A* l_pFromItem = dynamic_cast<A*>(*fromIt);
      
      l_bSuccess = (l_bSuccess && (l_pFromItem != NULL));
      
      if (l_pFromItem)
         to.push_back(l_pFromItem);
   }
   
   return l_bSuccess;
}

#endif // GENERIC_MODEL_H

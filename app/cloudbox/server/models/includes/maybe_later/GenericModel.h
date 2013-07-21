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

class GenericModel
{
public:
   
   /**
    * A model created without a connection can only be used for querying.
    */
   GenericModel(const std::string   &collectionName,
                MongoConnection*    pConnection = NULL);
         
   virtual ~GenericModel();
   
//   typedef std::vector<GenericModel> GenericModelList;
   static bool find(MongoConnection* pConnection,
                    const GenericModel& query,
                    std::vector<GenericModel*> &list);
   
   std::string collection() const;
   
   void setConnection(MongoConnection *pConnection);
   
   virtual bool insert();
   
   /**
    * Updates all fields.
    */
   virtual bool update();
   
   virtual bool append() { return true; };
   
   virtual bool remove();
   
   std::string idToString() const;
   
   /**
    * Indicates whether the 
    */
   bool isValid() const;
   
   bool find(mongo::BSONObj query,
             std::vector<GenericModel*> &list);
   
   bool findOne(mongo::BSONObj query,
                GenericModel &result);
      
protected:
   
   virtual GenericModel* create(MongoConnection* pConnection) const = 0;
   
   mongo::BSONObj object() const;
   
   std::string queryName(const std::string &dbName);
   
   
   
   bool incrementField(mongo::Query query, const std::string &counter);
   
   /**
    * If the Model has an OID, reload() updates the BSONObj with
    * the document from the database.
    */
   bool reload();
   
protected:
   
   MongoConnection*    m_pDbConn;
   
   std::string         m_sCollectionName;
   
   mongo::BSONObj      m_bsonObject;
   
};

template <class A>
bool convert(std::vector<A*> &to, const std::vector<GenericModel*> &from) {
   bool l_bSuccess = true;
   std::vector<GenericModel*>::const_iterator fromIt = from.begin();
   for (; fromIt != from.end(); fromIt++)
   {
      A* l_pFromItem = dynamic_cast<A*>(*fromIt);
      l_bSuccess = (l_bSuccess && (l_pFromItem != NULL));
      if (l_pFromItem) to.push_back(l_pFromItem);
   }
   return l_bSuccess;
}

#endif // GENERIC_MODEL_H

#ifndef COLLECTION_H
#define COLLECTION_H

#include <vector>
#include <string>
#include "MongoConnection.h"
#include "GenericModel.h"

typedef std::vector<GenericModel*> ModelList;
typedef std::vector<ObjectId> IdList;

class Collection
{
public:
   
   Collection(MongoConnection &connection, const std::string &name);
   
   std::string name();
   
   std::string fullName();
   
   MongoConnection& connection();
   
   virtual bool insert(const GenericModel &model);
   
   virtual bool remove(const GenericModel &model);
   virtual bool remove(const mongo::BSONObj &obj, bool bJustOne = true);
   
   virtual bool update(const mongo::BSONObj &query,
                      const mongo::BSONObj &update);
   
   virtual bool update(const GenericModel &model);
   
   virtual bool find(const GenericModel &query,
                     ModelList    &matches);
   
   virtual bool find(const mongo::BSONObj &query,
                    ModelList    &matches);
   
   virtual bool find(const mongo::BSONObj &obj, IdList &list);
   
   virtual bool findOne(const GenericModel &query, GenericModel &result);
   
   virtual bool findOne(const mongo::BSONObj &query,
                       GenericModel &result);
   
   virtual bool findAndModify(const mongo::BSONObj &query,
                              const mongo::BSONObj &update,
                              mongo::BSONObj &result,
                              bool  bRemove = false);
   
   bool increment(GenericModel &model,
                  const std::string &counter,
                  bool bUpdate = false);
   
   bool add(const mongo::BSONObj &query, 
            const std::string &array,
            const ObjectId &item, 
            bool bUnique = false);
   
   //bool add(const mongo::BSONObj &query, const std::string &array,
//            const mongo::BSONObj &item, bool bUnique = false);
   
   bool remove(const mongo::BSONObj &query, 
               const std::string &array,
               const ObjectId &item,
               bool bFirst = false);
   
   //bool remove(const mongo::BSONObj &query, const std::string &array,
//               const mongo::BSONObj &item, bool bFirst = false);
   
protected:
   
   MongoConnection* connectionPtr();
   
   virtual GenericModel* createModel() = 0;
   
   virtual GenericModel* createModel(const mongo::BSONObj &obj) = 0;
   
private:
   
   MongoConnection  &m_rConnection;
   
   std::string       m_sName;
};

#endif // COLLECTION_H
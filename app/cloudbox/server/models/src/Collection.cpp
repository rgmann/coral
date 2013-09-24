#include <iostream>
#include "Collection.h"

//------------------------------------------------------------------------------
Collection::Collection(MongoConnection &connection, const std::string &name)
: m_rConnection(connection)
, m_sName(name)
{
}

//------------------------------------------------------------------------------
std::string Collection::name()
{
   return m_sName;
}

//------------------------------------------------------------------------------
std::string Collection::fullName()
{
   return connection().dbName() + "." + name();
}

//------------------------------------------------------------------------------
MongoConnection& Collection::connection()
{
   return m_rConnection;
}

//------------------------------------------------------------------------------
bool Collection::insert(const GenericModel &model)
{
   if (!connection().isConnected()) return false;
   
   // Only insert objects that don't have a OID yet.
   if (model.isValid()) return false;
   
   connection().mongo().insert(fullName().c_str(), model.object());
   
   return true;
}

//------------------------------------------------------------------------------
bool Collection::remove(const GenericModel &model)
{
   if (!connection().isConnected()) return false;
      
   remove(model.object(), model.isValid());
   
   return true;
}

//------------------------------------------------------------------------------
bool Collection::remove(const mongo::BSONObj &obj, bool bJustOne)
{
   if (!connection().isConnected()) return false;
   
   connection().mongo().remove(fullName().c_str(), obj, bJustOne);
   
   return true;
}

//------------------------------------------------------------------------------
bool Collection::update(const mongo::BSONObj &query,
                        const mongo::BSONObj &update)
{
   if (!connection().isConnected()) return false;
   
   connection().mongo().update(fullName().c_str(), query, update);
   
   return true;
}

//------------------------------------------------------------------------------
bool Collection::update(const GenericModel &model)
{
   mongo::BSONElement l_objID;
   mongo::BSONObj     l_queryObj;
   
   if (!connection().isConnected()) return false;
   
   if (!model.isValid())
   {
      l_queryObj = model.object();
   }
   else
   {
      model.object().getObjectID(l_objID);
      l_queryObj = BSON("_id" << l_objID);
   }

   connection().mongo().update(fullName().c_str(), l_queryObj, model.object());
   
   return true;
}

//------------------------------------------------------------------------------
bool Collection::find(const GenericModel &query, ModelList &matches)
{
   return find(query.object(), matches);
}

//------------------------------------------------------------------------------
bool Collection::find(const mongo::BSONObj &query, ModelList &matches)
{
   mongo::auto_ptr<mongo::DBClientCursor> cursor;
   
   if (!connection().isConnected()) return false;
   
   cursor = connection().mongo().query(fullName().c_str(), query);
   while (cursor->more())
   {
      GenericModel* pModel = createModel(cursor->next());
      matches.push_back(pModel);
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool Collection::find(const mongo::BSONObj &query, IdList &matches)
{
   mongo::auto_ptr<mongo::DBClientCursor> cursor;
   
   if (!connection().isConnected()) return false;
   
   cursor = connection().mongo().query(fullName().c_str(), query);
   while (cursor->more())
   {
      mongo::BSONElement lObjectId;
      if (cursor->next().getObjectID(lObjectId) && 
          lObjectId.type() == mongo::jstOID)
      {
         matches.push_back(lObjectId.OID());
      }
   }
   
   return true;
}

//------------------------------------------------------------------------------
MongoConnection* Collection::connectionPtr()
{
   return &m_rConnection;
}

//------------------------------------------------------------------------------
bool Collection::findOne(const GenericModel &query, GenericModel &result)
{
   return findOne(query.object(), result);
}

//------------------------------------------------------------------------------
bool Collection::findOne(const mongo::BSONObj &query, GenericModel &result)
{
   mongo::BSONObj lResultObj;
   
   if (!connection().isConnected()) return false;
   
   lResultObj = connection().mongo().findOne(fullName().c_str(), query);
   
   if (!lResultObj.isEmpty())
   {
      result.object(lResultObj);
   }
   
   return (!lResultObj.isEmpty());
}

//------------------------------------------------------------------------------
bool Collection::findAndModify(const mongo::BSONObj &query,
                               const mongo::BSONObj &update,
                               mongo::BSONObj &result,
                               bool  bRemove)
{
   bool                    lbSuccess = false;
   mongo::BSONObjBuilder   lCommand;
   
   if (!connection().isConnected()) return false;
   
   lCommand << "findandmodify" << name() << 
               "query" << query << 
               "update" << update << 
               "new" << 0;
   
   lbSuccess = connection().mongo().runCommand(connection().dbName(),
                                               lCommand.obj(),
                                               result);
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool Collection::increment(GenericModel &model,
                           const std::string &counter,
                           bool bUpdate)
{
   bool lbSuccess = false;
   mongo::BSONElement lObjectId;

   if (!connection().isConnected()) return false;
   
   if (model.object().getObjectID(lObjectId))
   {
      connection().mongo().update(fullName().c_str(), BSON("_id" << lObjectId), 
                               BSON("$inc" << BSON(counter << 1)));
      lbSuccess = true;
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool Collection::add(const mongo::BSONObj &query, 
                     const std::string &array,
                     const ObjectId &item,
                     bool bUnique)
{
   bool lbSuccess = false;
   
   if (!connection().isConnected()) return false;
   
   connection().mongo().update(fullName().c_str(),
                               query,
                               BSON("$push" << BSON(array << item)));
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool Collection::remove(const mongo::BSONObj &query, 
                        const std::string &array,
                        const ObjectId &item,
                        bool bFirst)
{
   bool lbSuccess = false;
   
   if (!connection().isConnected()) return false;
   
   connection().mongo().update(fullName().c_str(),
                               query,
                               BSON("$pull" << BSON(array << item)));
   
   return lbSuccess;
}

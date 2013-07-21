#include "GenericModel.h"

//------------------------------------------------------------------------------
GenericModel::GenericModel(const std::string &collectionName,
                           MongoConnection*   pConnection)
: m_pDbConn(pConnection)
, m_sCollectionName(collectionName)
{
}

//------------------------------------------------------------------------------
GenericModel::~GenericModel()
{
}

//------------------------------------------------------------------------------
void GenericModel::setConnection(MongoConnection *pConnection)
{
   m_pDbConn = pConnection;
}

//------------------------------------------------------------------------------
std::string GenericModel::collection() const
{
   return m_sCollectionName;
}

//------------------------------------------------------------------------------
bool GenericModel::find(MongoConnection* pConnection,
                        const GenericModel   &query,
                        std::vector<GenericModel*>     &results)
{
   std::string l_sQueryName = "";
   mongo::auto_ptr<mongo::DBClientCursor> cursor;
   
   if (!pConnection->isConnected()) return false;

   l_sQueryName = pConnection->dbName() + "." + query.collection();
   cursor = pConnection->mongo().query(l_sQueryName,
                                       query.object());
   while (cursor->more())
   {
//      GenericModel* pModel = new GenericModel(query.collection(), pConnection);
      GenericModel* pModel = query.create(pConnection);
//      GenericModel l_model(query.collection(), pConnection);
//      l_model.m_bsonObject = cursor->next();
      pModel->m_bsonObject = cursor->next();
      
//      results.push_back(l_model);
      results.push_back(pModel);
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool GenericModel::find(mongo::BSONObj query,
                        std::vector<GenericModel*>     &results)
//                        std::vector<GenericModel> &results)
{
   std::string l_sQueryName = "";
   mongo::auto_ptr<mongo::DBClientCursor> cursor;
   
   if (!m_pDbConn->isConnected()) return false;
   
   l_sQueryName = m_pDbConn->dbName() + "." + collection();
   cursor = m_pDbConn->mongo().query(l_sQueryName, query);
   
   while (cursor->more())
   {
//      GenericModel l_model(collection, pConnection);
//      GenericModel* pModel = new GenericModel(collection, pConnection);
      GenericModel* pModel = create(m_pDbConn);

//      l_model.m_bsonObject = cursor->next();
      pModel->m_bsonObject = cursor->next();
      
//      results.push_back(l_model);
      results.push_back(pModel);
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool GenericModel::findOne(mongo::BSONObj query, GenericModel &result)
{
   std::string l_sCollection = "";
   mongo::auto_ptr<mongo::DBClientCursor> cursor;
   
   if (!m_pDbConn->isConnected()) return false;
   
   l_sCollection = m_pDbConn->dbName() + "." + result.collection();
   result.m_bsonObject = m_pDbConn->mongo().findOne(l_sCollection, query);
   
   // Make sure the GenericModel instance has a data base connection.
   result.setConnection(m_pDbConn);
   
   return result.m_bsonObject.hasField("_id");
}

//------------------------------------------------------------------------------
bool GenericModel::insert()
{
   mongo::BSONObj l_object;
   
   if (!m_pDbConn->isConnected()) return false;
   
   // Only insert objects that don't have a OID yet.
   if (isValid()) return false;
   
   m_pDbConn->mongo().insert(queryName(m_pDbConn->dbName()).c_str(),
                             m_bsonObject);
   
   return true;
}

//------------------------------------------------------------------------------
bool GenericModel::update()
{
   bool l_bSuccess = false;
   
   if (!m_pDbConn->isConnected()) return false;
   
   if (isValid())
   {
      mongo::BSONElement l_element;
      
      l_bSuccess = m_bsonObject.getObjectID(l_element);
      
      if (l_bSuccess)
      {
         m_pDbConn->mongo().update(queryName(m_pDbConn->dbName()).c_str(),
                                BSON("_id" << l_element),
                                m_bsonObject);
      }
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool GenericModel::remove()
{   
   if (!m_pDbConn->isConnected()) return false;
   
   if (isValid())
   {
      m_pDbConn->mongo().remove(queryName(m_pDbConn->dbName()).c_str(),
                                m_bsonObject);
   }
   
   return true;
}

//------------------------------------------------------------------------------
std::string GenericModel::idToString() const
{
   mongo::BSONElement l_element;
   
   if (m_bsonObject.getObjectID(l_element))
      return l_element.toString();
   else
      return std::string("NOT_SET");
}

//------------------------------------------------------------------------------
mongo::BSONObj GenericModel::object() const
{
   return m_bsonObject;
}

//------------------------------------------------------------------------------
std::string GenericModel::queryName(const std::string &dbName)
{
   return m_pDbConn->dbName() + "." + m_sCollectionName;
}

//------------------------------------------------------------------------------
bool GenericModel::incrementField(mongo::Query     query,
                                  const std::string &counter)
{
   if (!m_pDbConn->isConnected()) return false;
   
   m_pDbConn->mongo().update(queryName(m_pDbConn->dbName()).c_str(),
                             query, BSON("$inc" << BSON(counter << 1)));
   
   return true;
}

//------------------------------------------------------------------------------
bool GenericModel::reload()
{
   bool l_bSuccess = false;
   mongo::BSONElement l_element;
   
   if (m_bsonObject.getObjectID(l_element))
   {
      m_bsonObject = m_pDbConn->mongo().findOne(queryName(
                                 m_pDbConn->dbName()).c_str(),
                                 BSON("_id" << l_element));
      
      l_bSuccess = true;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool GenericModel::isValid() const
{
   return m_bsonObject.hasField("_id");
}

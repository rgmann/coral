#include "GenericModel.h"
#include "Collection.h"

//------------------------------------------------------------------------------
GenericModel::GenericModel(const std::string &modelName)
: m_sModelName(modelName)
{
}

//------------------------------------------------------------------------------
GenericModel::~GenericModel()
{
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
std::string GenericModel::modelName()
{
   return m_sModelName;
}

//------------------------------------------------------------------------------
std::string GenericModel::collectionName()
{
   return modelName() + "s";
}

//------------------------------------------------------------------------------
const mongo::BSONObj& GenericModel::object() const
{
   return m_bsonObject;
}

//------------------------------------------------------------------------------
mongo::BSONObj& GenericModel::object()
{
   return m_bsonObject;
}

//------------------------------------------------------------------------------
void GenericModel::object(const mongo::BSONObj &obj)
{
   m_bsonObject = obj;
}

//------------------------------------------------------------------------------
//bool GenericModel::reload()
//{
//   bool l_bSuccess = false;
//   mongo::BSONElement l_element;
//   
//   if (m_pConnection == NULL) return false;
//   if (!m_pConnection->isConnected()) return false;
//   
//   if (m_bsonObject.getObjectID(l_element))
//   {
//      m_bsonObject = m_pConnection->mongo().findOne(
//                                 collectionName().c_str(),
//                                 BSON("_id" << l_element));
//      
//      l_bSuccess = true;
//   }
//   
//   return l_bSuccess;
//}

//------------------------------------------------------------------------------
bool GenericModel::isValid() const
{
   return m_bsonObject.hasField("_id");
}

//------------------------------------------------------------------------------
mongo::BSONElement GenericModel::getObjectId()
{
   mongo::BSONElement lObjectId;
   
   if (!object().getObjectID(lObjectId))
   {
      lObjectId = mongo::BSONElement();
   }
   
   return lObjectId;
}

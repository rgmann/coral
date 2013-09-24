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
void GenericModel::clear()
{
   object(mongo::BSONObj());
}

//------------------------------------------------------------------------------
bool GenericModel::isValid() const
{
   return m_bsonObject.hasField("_id");
}

//------------------------------------------------------------------------------
bool GenericModel::getObjectId(ObjectId &objectId) const
{
   bool lbSuccess = false;
   mongo::BSONElement lIdElement;
   
   objectId.clear();
   
   if (object().getObjectID(lIdElement) && 
       lIdElement.type() == mongo::jstOID)
   {
      objectId = lIdElement.OID();
      lbSuccess = true;
   }
   
   return lbSuccess;
}

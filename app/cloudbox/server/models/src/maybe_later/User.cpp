#include "User.h"
#include "Group.h"

//------------------------------------------------------------------------------
User::User(MongoConnection* pConnection)
: GenericModel("users", pConnection)
{
}

//------------------------------------------------------------------------------
User::~User()
{
}

//------------------------------------------------------------------------------
void User::name(const std::string &first, const std::string &last)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("firstName", BSON("first" << first << "last" << last));
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
std::pair<std::string, std::string> User::name()
{
   mongo::BSONObj nameObject;
   
   nameObject = m_bsonObject.getObjectField("name");
   
   return std::make_pair(nameObject.getStringField("first"),
                         nameObject.getStringField("last"));
}

//------------------------------------------------------------------------------
void User::login(const std::string &loginId)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("loginId", loginId);
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
std::string User::login()
{
   return std::string(m_bsonObject.getStringField("loginId"));
}

//------------------------------------------------------------------------------
bool User::groups(std::vector<Group*> &groups)
{
   bool l_bSuccess = false;
   mongo::BSONElement l_objectId;
   Group l_group(m_pDbConn);
   std::vector<GenericModel*> l_vResults;
   
   if (!m_bsonObject.getObjectID(l_objectId)) return false;
   
   l_bSuccess = l_group.find(BSON("members" << l_objectId),
                             l_vResults);
   
   if (l_bSuccess)
   {
      l_bSuccess = convert<Group>(groups, l_vResults);
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
GenericModel* User::create(MongoConnection* pConnection) const
{
   return new User(pConnection);
}

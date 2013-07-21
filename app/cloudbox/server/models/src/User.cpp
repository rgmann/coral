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
   l_objBuilder.append("name", BSON("first" << first << "last" << last));
   
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

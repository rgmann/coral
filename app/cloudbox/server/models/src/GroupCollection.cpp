#include "GroupCollection.h"

//------------------------------------------------------------------------------
GroupCollection::GroupCollection(MongoConnection &connection)
: Collection(connection, "groups")
{
}

//------------------------------------------------------------------------------
bool GroupCollection::getMembers(const Group &group, std::vector<Group*> &groups)
{
   return false;
}

//------------------------------------------------------------------------------
bool GroupCollection::addMember(const Group &group, const User &user)
{
   return false;
}

//------------------------------------------------------------------------------
bool GroupCollection::getFiles(const Group &group, std::vector<File*> &files)
{
   return false;
}

//------------------------------------------------------------------------------
GenericModel* GroupCollection::createModel()
{
   return new Group();
}

//------------------------------------------------------------------------------
GenericModel* GroupCollection::createModel(const mongo::BSONObj &object)
{
   Group* lpGroup = new Group();
   lpGroup->object(object);
   return lpGroup;
}

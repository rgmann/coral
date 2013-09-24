#include "User.h"
#include "UserCollection.h"
#include "GroupCollection.h"

//------------------------------------------------------------------------------
GroupCollection::GroupCollection(MongoConnection &connection)
: Collection(connection, "groups")
{
}

//------------------------------------------------------------------------------
bool GroupCollection::create(Group &group, const User &owner)
{
   mongo::BSONObjBuilder lBuilder;
   mongo::BSONObj lGroup;
   ObjectId lOwnerId;
   
   if (!owner.getObjectId(lOwnerId)) return false;
   
   lBuilder.append(Group::GroupOwnerField, lOwnerId);
   lBuilder.append(Group::GroupRevField, 0);
   lGroup = lBuilder.obj();
   
   connection().mongo().insert(fullName().c_str(), lGroup);
   if (!findOne(lGroup, group)) return false;
   
   //group.object(lUpdatedGroup);
   
   return true;//insert(group);
}

//------------------------------------------------------------------------------
bool GroupCollection::getGroup(const ObjectId &objId, Group &group)
{
   //bool lbSuccess = false;
//   mongo::BSONObj lObject;
      
   return findOne(BSON("_id" << objId), group);
   //{
//      group.object(lObject);
//   }
   
   //return lbSuccess;
}

//------------------------------------------------------------------------------
bool GroupCollection::getMembers(const ObjectId &groupId, 
                                 std::vector<User> &members)
{
   Group lGroup;
   UserCollection lUserCollection(connection());
   
   if (!findOne(BSON("_id" << groupId), lGroup))
   {
      std::cout << "Failed to find group: " 
               << BSON("_id" << groupId).toString() << std::endl;
      return false;
   }
   
   if (!lGroup.object().hasField(Group::GroupMembersField))
   {
      std::cout << "Group does not have members field." << std::endl;
      return false;
   }
   
   std::vector<mongo::BSONElement> lvMemberIds;
   lvMemberIds = lGroup.object().getField(Group::GroupMembersField).Array();
   
   std::vector<mongo::BSONElement>::iterator liMemberId = lvMemberIds.begin();
   std::cout << "Found " << lvMemberIds.size() << " members." << std::endl;
   for (; liMemberId != lvMemberIds.end(); liMemberId++)
   {
      //std::cout << "Type " << liMemberId->type() << std::endl;
      User lMember;
      if (liMemberId->type() == mongo::jstOID &&
          lUserCollection.findOne(BSON("_id" << liMemberId->OID()), lMember))
      {
         if (lMember.isValid()) members.push_back(lMember);
      }
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool GroupCollection::addMember(const Group &group, const User &user)
{
   Group lGroup;
   ObjectId lUserId;
   
   if (!user.getObjectId(lUserId)) return false;

   // group may be supplied with a minimum of fields populated.  Therefore,
   // query for a fully populated group.
   if (!findOne(group, lGroup)) return false;
   
   return add(lGroup.object(), 
              std::string(Group::GroupMembersField), 
              lUserId, true);   
}

//------------------------------------------------------------------------------
bool GroupCollection::removeMember(const Group &group, const User &user)
{
   Group lGroup;
   ObjectId lUserId;
   
   if (!user.getObjectId(lUserId)) return false;
   
   // group may be supplied with a minimum of fields populated.  Therefore,
   // query for a fully populated group.
   if (!findOne(group, lGroup)) return false;
   
   return remove(lGroup.object(), 
                 std::string(Group::GroupMembersField), 
                 lUserId, true);   
}

//------------------------------------------------------------------------------
//bool GroupCollection::getFiles(const Group &group, std::vector<File*> &files)
//{
//   return false;
//}

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

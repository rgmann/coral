#ifndef GROUP_COLLECTION_H
#define GROUP_COLLECTION_H

#include "Collection.h"
#include "Group.h"

class User;
class File;

class GroupCollection : public Collection
{
public:
   
   GroupCollection(MongoConnection &connection);
   
   bool create(Group &group, const User &owner);
   
   bool getGroup(const ObjectId &objId, Group &group);
   
   /**
    * Returns full list of groups that this user is a member of.
    */
   bool getMembers(const ObjectId &groupId, std::vector<User> &members);
   
   bool addMember(const Group &group, const User &user);

   bool removeMember(const Group &group, const User &user);
   
   bool getFiles(const Group &group, std::vector<File*> &files);
   
   bool incrementRevision();
   
protected:
   
   virtual GenericModel* createModel();
   
   virtual GenericModel* createModel(const mongo::BSONObj &object);
};

#endif // GROUP_COLLECTION_H
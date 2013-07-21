#ifndef GROUP_COLLECTION_H
#define GROUP_COLLECTION_H

#include "Collection.h"
#include "User.h"

class GroupCollection : public Collection
{
public:
   
   GroupCollection(MongoConnection &connection);
   
   /**
    * Returns full list of groups that this user is a member of.
    */
   bool getMembers(const Group &group, std::vector<Group*> &groups);
   
   bool addMember(const Group &group, const User &user);
   
   bool getFiles(const Group &group, std::vector<File*> &files);
   
protected:
   
   virtual GenericModel* createModel();
   
   virtual GenericModel* createModel(const mongo::BSONObj &object);
};

#endif // GROUP_COLLECTION_H
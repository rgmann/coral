#ifndef USER_COLLECTION_H
#define USER_COLLECTION_H

#include "Collection.h"
#include "User.h"

class Group;
class File;

class UserCollection : public Collection
{
public:
   
   UserCollection(MongoConnection &connection);
   
   /**
    * Returns full list of groups that this user is a member of.
    */
   bool getGroups(const User &user, std::vector<Group*> &groups);
   
   bool getFiles(const User &user, std::vector<File*> &files);
   
protected:
   
   virtual GenericModel* createModel();
   
   virtual GenericModel* createModel(const mongo::BSONObj &object);
};

#endif // USER_COLLECTION_H
#ifndef USER_H
#define USER_H

#include <string>
#include <utility>
//#include "GroupClock.h"
#include "GenericModel.h"

class Group;

class User : public GenericModel
{
public:
      
   explicit User(MongoConnection* pConnection = NULL);

   ~User();

   void name(const std::string &first, const std::string &last);
   std::pair<std::string, std::string> name();

   void login(const std::string &loginId);
   std::string login();
   
   /**
    * Returns full list of groups that this user is a member of.
    */
   bool groups(std::vector<Group*> &groups);
   
   /**
    * Returns user's root group.
    */
   bool rootGroup(Group &rootGroup);
   
   /**
    * Returns the set of groups with clocks that have been incremented,
    * signifying an update, since the client last requested an update.
    * The group clock is is updated each time the server file changed in
    * response a client file change notification.  Note: the group clock is
    * incremented after a file is unlocked following a write.
    */
   //bool updatedGroups(std::vector<Group> &groups);
   //bool groupClocks(std::vector<GroupClocks> &groupClocks);
   
   //bool files(std::vector<File> &files);
   
   //bool getClock(const Group &group, GroupClock &clock);
   //bool getVectorClock(GroupVectorClock &vector);
protected:
   
   GenericModel* create(MongoConnection* pConnection) const;
};

#endif // USER_H

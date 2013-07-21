#ifndef USER_H
#define USER_H

#include <string>
#include <utility>
#include "GenericModel.h"

class Group;

class User : public GenericModel
{
public:
      
   User();

   ~User();

   void name(const std::string &first, const std::string &last);
   std::pair<std::string, std::string> name();

   void login(const std::string &loginId);
   std::string login();
   
   Date lastActive();
   
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
      
   //bool getClock(const Group &group, GroupClock &clock);
   //bool getVectorClock(GroupVectorClock &vector);
protected:
   
};

#endif // USER_H

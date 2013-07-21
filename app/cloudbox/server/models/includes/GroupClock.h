#ifndef GROUP_CLOCK_H
#define GROUP_CLOCK_H

#include <vector>
#include "GenericModel.h"

class Group;
class User;

/**
 * The usergroupclock collection contains one document for each user/group
 * membership.  Thus, if user B has a root directory and has been added to one
 * of user A's groups, user B will have two clock documents in the 
 * "usergroupclock" collection.  The clock indicates the group revision number
 * at which the the user last synhronized with the group.
 */
class GroupClock : public GenericModel
{
public:

   explicit GroupClock(MongoConnection *pConnection = NULL);
   
   ~GroupClock();
   
   void group(const Group &group);
   
   void user(const User &user);
   
   bool increment();
   ui64 clock();
   
   enum CompStatus {
      LessThan,
      Equal,
      GreaterThan
   };
   CompStatus compare();
};

typedef std::vector<GroupClock> GroupVectorClock;

#endif // GROUP_CLOCK_H
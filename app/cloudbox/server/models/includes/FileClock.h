#ifndef FILE_CLOCK_H
#define FILE_CLOCK_H

#include <vector>
#include "ClockModel.h"
#include "VectorClock.h"

class File;
class User;

/**
 * The usergroupclock collection contains one document for each user/group
 * membership.  Thus, if user B has a root directory and has been added to one
 * of user A's groups, user B will have two clock documents in the 
 * "usergroupclock" collection.  The clock indicates the group revision number
 * at which the the user last synhronized with the group.
 */
class FileClock : public ClockModel
{
public:

   explicit FileClock(MongoConnection *pConnection = NULL);
   
   ~FileClock();
   
   void file(const Group &group);
   
   void user(const User &user);
};

class FileVectorClock : VectorClock
{
public:
   
   FileVectorClock(const User &user, const File &file);
   
   bool load();
   
};


#endif // GROUP_CLOCK_H
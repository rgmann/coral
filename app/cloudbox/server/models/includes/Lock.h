#ifndef LOCK_H
#define LOCK_H

#include <time.h>
#include "User.h"

typedef mongo::BSONElement UserID;

class WriteLock
{
public:
   
   Lock(User &user);
   
   Lock(const UserID &userID)
   
   time_t time();
   
   UserID userId();
   
   bool valid();
      
private:
   
   UserID   myOwnerId;
   
   time_t   myLockTime;
   
   bool     mybIsValid;
};

class ReadLock
{
public:
   
   ReadLock();
   
   
};

#endif // LOCK_H
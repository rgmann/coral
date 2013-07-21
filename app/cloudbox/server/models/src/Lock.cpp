#include "Lock.h"

//------------------------------------------------------------------------------
Lock::Lock(User &user)
{
   mybIsValid = user.object().getObjectID(myOwnerId);
   if (mybIsValid) time(&myLockTime);
}

//------------------------------------------------------------------------------
Lock::Lock(const UserID &userId)
{
   mybIsValid = (userId.type() == mongo::jstOID);
   if (mybIsValid) time(&myLockTime);
}

//------------------------------------------------------------------------------
time_t Lock::time()
{
   return myLockTime;
}

//------------------------------------------------------------------------------
UserID Lock::userId()
{
   return myOwnerId;
}

//------------------------------------------------------------------------------
bool Lock::valid()
{
   return mybIsValid;
}

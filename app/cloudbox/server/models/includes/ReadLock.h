#ifndef READ_LOCK_H
#define READ_LOCK_H

#include "GenericModel.h"

class ReadLock
{
public:

   ReadLock();

   ReadLock(const mongo::BSONObj &lockObj);

   typedef std::pair<ObjectId, ui64> LockMember;
   bool getMembers(std::vector<LockMember> &members) const;

   mongo::BSONObj toObject() const;

private:

   mongo::BSONObj mLockObject;
};

#endif // READ_LOCK_H

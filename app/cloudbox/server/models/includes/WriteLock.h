#ifndef WRITE_LOCK_H
#define WRITE_LOCK_H

#include "GenericModel.h"

class WriteLock
{
public:

   WriteLock();

   WriteLock(const ObjectId &owner);

   WriteLock(const ObjectId &lock);

   bool getOwner(ObjectId &owner) const;

   bool getLockTime(ui64 &ts) const;

   mongo::BSONObj toObject() const;

private:

   mongo::BSONObj mLockObject;
};

#endif // WRITE_LOCK_H

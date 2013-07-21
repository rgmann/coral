#ifndef FILE_COLLECTION_H
#define FILE_COLLECTION_H

#include "File.h"
#include "Collection.h"

class User;
class Group;

class FileCollection : public Collection
{
public:
   
   FileCollection(MongoConnection &connection);
   
   /**
    * Lock a file for reading.  Atomically compares the user clock against the
    * the requested file's clock.  If the users clock "happened before" the
    * file clock, the file is locked and returned in the file object.  If the
    * user clock is identical to the file clock or it "happend after" the file
    * clock, lockForRead() returns false.
    *
    * Precondition: The OID of the file instance has been populated.
    */
   bool lockForRead(File &file, User &user);
   
   /**
    * Lock a file for writing.  Atomically compares the user clock against the
    * requested file's clock.  If the user's clock is identical to the file's
    * clock, then a conflict is implied by the fact that a write only happens
    * if user's copy of a file has changed locally.  In this case, the clocks
    * can only be identical if another user updated the file before this user
    * could.  If the user's clock happened before the file's clock, the lock
    * fails because the files are obviously conflicted.  If the user's clock
    * happened after the file's clock, the file is locked for updating.
    *
    * Note: User should always perform a lockForUpdate() before a create
    *       to maintain a consistent file creation process. (TBD)
    *
    * @param   lock - invalid lock instance; modified by lockForUpdate() if the
    *                 file exists; On exit, lock contains the lock user, etc.
    *
    * @param   user - user that is requesting the lock
    *
    * @param   path - file path
    *
    * @param   file - blank file instance that is populated by lockForUpdate();
    *                 Note: only valid on success.
    *
    * @return  LockStatus - Indicates the status of the file lock attempt
    */
   bool lockForUpdate(File &file, const User &user, const FilePath &path);
   
   /**
    * Create a new file.  If the file exists already.  A file is created with
    * the creators user name appended "_conflict_username".  In this case, the
    * File instance will contain the conflict path.  Also, a document is
    * created in the conflict collection linking the conflicting file with the
    * file it conflicted with.
    *
    * If there is no conflict, a new file entry is created and locked.
    */
   bool create(User &user, const FilePath &path, File &file);
   
   /**
    * Unlock a currently locked file, regardless of whether it was locked for
    * reading or writing.  Unlock verifies the lock before unlocking the file.
    * It also verifies that the file is closed.
    */
   bool unlock(File &file, User &user);
   
   bool getReadRef(User &user, std::ifstream &rstream);
   
   bool getWriteRef(User &user, std::ofstream &wstream);
   
   //bool setReadLock(File &file, const ReadLock &lock);
//   
//   bool setWriteLock(File &file, const WriteLock &lock);
   
private:
   
   virtual GenericModel* createModel();
   
   virtual GenericModel* createModel(const mongo::BSONObj &object);
};

#endif // FILE_COLLECTION_H

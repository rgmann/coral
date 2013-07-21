#ifndef FILE_H
#define FILE_H

#include "GenericModel.h"
#include "FilePath.h"

class User;
class Group;
class Lock;

class File : GenericModel
{
   File();
   
   enum Type {
      UnknownType,
      FileType,
      DirectoryType,
      LinkType
   };
   
   Type type() const;
   
   /**
    * File extension.
    */
   std::string ext();
   
   ui64 size();

   /**
    * All files are stored in one directory as "fileobjectid.file"
    */
   bool getPath(FilePath);
   
   void setPath(const FilePath &path);

   /**
    * Returns true if the file is locked. A file is locked if the lock field is
    * not mongo::jstNULL.
    */
   bool isLocked() const;
   
   enum LockStatus {
      FileNotFound,
      AlreadyLocked,
      LockFailure,
      Conflict
   };
   
   enum LockError {
      
   };
   
   enum LockType {
      None,
      ReadLock, // Any number of reads,  no writers
      WriteLock // No readers, one writer
   };
   
   LockType isLockedByUser(const User &user);
   
};

#endif // FILE_H
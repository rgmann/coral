#ifndef FILE_H
#define FILE_H

#include "GenericModel.h"
#include "FilePath.h"

class User;
class Group;

class File : public GenericModel
{
public:
   
   File();
   
   enum Type {
      Unknown,
      Regular,
      Directory,
      Link
   };
   
   void setType(Type type);
   
   bool getType(Type &type) const;
   
   
   static std::string TypeToString(Type type);
   
   static File::Type StringToType(const std::string &type);
   
   
   void setPath(const std::string &path);
   
   bool getPath(std::string &path) const;
   
   
   void setSize(ui64 size)
   
   bool getSize(ui64 &size) const;
   
   
   bool setParent(const ObjectId &parentId);
   
   bool getParent(ObjectId &parentId) const;
   
   
   bool setOwner(const ObjectId &ownerId);
   
   bool getOwner(ObjectId &ownerId) const;
   
   
   void setGroup(const ObjectId &groupId);
   
   bool getGroup(ObjectId &groupId) const;
   
   /**
    * All files are stored in one directory as "fileobjectid.file"
    */
   void setPath(const FilePath &path);

   /**
    * Returns true if the file is locked. A file is locked if the lock field is
    * not mongo::jstNULL.
    */
   bool isLockedForRead() const;
   
   bool getReadLock(ReadLock &lock) const;
   
   bool isLockedForWrite() const;
   
   bool getWriteLock(WriteLock &lock) const;
   
};

#endif // FILE_H
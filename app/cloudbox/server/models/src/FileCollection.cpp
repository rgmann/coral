#include "UserCollection.h"
#include "GroupCollection.h"
#include "FileCollection.h"

//------------------------------------------------------------------------------
FileCollection::FileCollection(MongoConnection &connection)
: Collection(connection, "files")
{
}

//------------------------------------------------------------------------------
bool FileCollection::lockForRead(File &file, User &user)
{
   bool lbSuccess = false;
   mongo::BSONObj lUpdatedFile;
   
   if (connection().isConnected() && file.isValid())
   {
      // First, check whether this file is locked for writing.  If this user has
      // already locked the file for writing.  Write lock is automatically
      // removed and then attempts to get read lock.
      unlockWrite(file, user);
      
      lbSuccess = findAndModify(BSON("_id" << file.getObjectId() << 
                                      "wlock.isLocked" << "false"),
                                 BSON("$inc" << BSON("rlock.count" << "1") <<
                                      "$push" << BSON("rlock.users" << 
                                                      user.getObjectId())),
                                lUpdatedFile);
      
      file.object(lUpdatedFile);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool FileCollection::lockForWrite(File &file, User &user)
{
   bool lbSuccess = false;
   mongo::BSONObj lUpdatedFile;

   if (connection().isConnected() && file.isValid())
   {
      // First, check whether this file is locked for writing.  If this user has
      // already locked the file for writing.  Write lock is automatically
      // removed and then attempts to get read lock.
      unlockRead(file, user);
      
      lbSuccess = findAndModify(BSON("_id" << file.getObjectId() << 
                                      "wlock.isLocked" << "false"),
                           BSON("$set" << BSON("wlock.isLocked" << "true") <<
                                      "$set" << BSON("rlock.user" << 
                                                   user.getObjectId())),
                                 lUpdatedFile);
      
      file.object(lUpdatedFile);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool FileCollection::createFile(User &user, const FilePath &path,
                                File::Type type, File &file)
{
   bool lbSuccess = false;
   
   GroupCollection groups(connection());
   ModelList  lvGroups;
   ModelList::iterator liGroup;
   
   File lParentFile;
   File lFile;
   
   FilePath baseDir(path - path.getLastSegment());
   
   if (!connection().isConnected())
   {
      std::cout << "Not connected" << std::endl;
      return false;
   }
   
   if (!user.isValid())
   {
      std::cout << "User not valid!" << std::endl;
      return false;
   }
   
   // Parse path to get path of parent file
   
   // Find the base directory. Not very efficient since we have
   // to index all files in all groups that this user belongs to.
   if (!groups.find(BSON("members" << user.getObjectId()),
                        lvGroups))
   {
      return false;
   }
   
   bool lbFoundParent = false;
   bool lbFileExists = false;
   
   liGroup = lvGroups.begin();
   for (; liGroup != lvGroups.end(); liGroup++)
   {
      lbFileExists = findOne(BSON("groups" << 
                              (*liGroup)->getObjectId()
                                    << "path" << path.getFullPath()),
                                        lFile);
      
      lbFoundParent = findOne(BSON("groups" << 
                                    (*liGroup)->getObjectId()
                                    << "path" << baseDir.getFullPath()),
                                         lParentFile);
      
      if (lbFoundParent) break;
   }
   
   if (!lbFoundParent) return false;
   if (lbFileExists) return false;
   if (liGroup == lvGroups.end()) return false;
   
   Group* lpGroup = reinterpret_cast<Group*> (*liGroup);
   
   // Check that members of this group are allowed to create files.
   //if (!lGroup->permissionsInclude(Group::WritePermissions)) return false;
   
   // Set the file attributes.
   //file.creator(user);
//   file.owner(user);
//   file.group(*lpGroup);
   file.type(type);
   //file.parent(lParentFile);
   file.setPath(path);
   
   Date lCreateDate;
   lCreateDate.sample();
   //file.createdDate(lCreateDate.sts());
//   file.lastModified(lCreateDate.sts());
   
   // On creation, the file is locked by the user that created it.
   //WriteLock lWriteLock;
//   lWriteLock.user(user);
//   lWriteLock.time(l_createDate.sts());
//   lWriteLock.lock();
//   setWriteLock(file, lWriteLock);
   insert(file);
   lockForWrite(file, user);
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool FileCollection::unlock(File &file, User &user)
{
   bool lbSuccess = false;
   
   if (connection().isConnected() && file.isValid())
   {
      lbSuccess |= unlockWrite(file, user);
      lbSuccess |= unlockRead(file, user);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool FileCollection::unlockWrite(File &file, User &user)
{
   bool l_bSuccess = false;
   mongo::BSONObj lUpdatedFile;
   
   if (connection().isConnected() && file.isValid())
   {
      l_bSuccess = findAndModify(BSON("_id" << file.getObjectId() << 
                                      "wlock.isLocked" << "true"),
                           BSON("$set" << BSON("wlock.isLocked" << "false") <<
                              "$set" << BSON("wlock.user" << mongo::jstNULL)),
                                 lUpdatedFile);
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool FileCollection::unlockRead(File &file, User &user)
{
   bool lbSuccess = false;
   mongo::BSONObj lUpdatedFile;
   
   if (connection().isConnected() && file.isValid())
   {
      lbSuccess = findAndModify(BSON("_id" << file.getObjectId() << 
                                    "rlock.count" << BSON( "$gt" << 0) <<
                                 "rlock.users" << user.getObjectId()),
         BSON("$pull" << BSON("rlock.users" << user.getObjectId()) <<
                                      "$inc" << BSON("rlock.count" << -1)),
                                 lUpdatedFile);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
GenericModel* FileCollection::createModel()
{
   return new File();
}

//------------------------------------------------------------------------------
GenericModel* FileCollection::createModel(const mongo::BSONObj &object)
{
   File* lpFile = new File();
   lpFile->object(object);
   return lpFile;
}

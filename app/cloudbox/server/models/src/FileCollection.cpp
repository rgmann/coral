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
   
   if (connection().isConnected() && file.isValid())
   {
      // First, check whether this file is locked for writing.  If this user has
      // already locked the file for writing.  Write lock is automatically
      // removed and then attempts to get read lock.
      unlockWrite(file, user);
      
      lbSuccess = findAndModify(BSON("_id" << file.object().getObjectID() << 
                                      "wlock.isLocked" << "false"),
                                 BSON("$inc" << BSON("rlock.count" << "1") <<
                                      "$push" << BSON("rlock.users" << 
                                                      user.object().getobjectID())),
                                 false,
                                 true);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool FileCollection::lockForWrite(File &file, User &user)
{
   bool lbSuccess = false;
   
   if (connection().isConnected() && file.isValid())
   {
      // First, check whether this file is locked for writing.  If this user has
      // already locked the file for writing.  Write lock is automatically
      // removed and then attempts to get read lock.
      unlockRead(file, user);
      
      lbSuccess = findAndModify(BSON("_id" << file.object().getObjectID() << 
                                      "wlock.isLocked" << "false"),
                           BSON("$set" << BSON("wlock.isLocked" << "true") <<
                                      "$set" << BSON("rlock.user" << 
                                                   user.object().getobjectID())),
                                 false,
                                 true);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool FileCollection::create(User &user,
                  const FilePath &path,
                  FileType type,
                  File &file)
{
   bool lbSuccess = false;
   
   GroupCollection groups;
   std::vector<Group*>   lvGroups;
   std::vector<Group>::iterator liGroup;
   
   File l_parentFile;
   File l_file;
   
   FilePath baseDir(path - path.getLastSegment());
   
   if (!connection->isConnected()) return false;
   if (!user.isValid()) return false;
   
   // Find the base directory. Not very efficient since we have to since we have
   // to index all files in all groups that this user belongs to.
   if (!groups.find(BSON("members" << user.object().getObjectID()),
                        groupList))
   {
      return false;
   }
   
   bool lbFoundParent = false;
   bool lbFileExists = false;
   
   liGroup = lvGroups.begin();
   for (; liGroup != lvGroups.end(); liGroup++)
   {
      lbFileExists = findOne(BSON("groups" << 
                              (*liGroup)->object().getObjectID()
                                    << "path" << path.getFullPath()),
                                        lFile);
      
      lbFoundParent = findOne(BSON("groups" << 
                                    (*liGroup)->object().getObjectID()
                                    << "path" << baseDir.getFullPath()),
                                         lParentFile);
      
      if (lbFoundParent) break;
   }
   
   if (!lbFoundParent) return false;
   if (lbFileExists) return false;
   if (liGroups == lvGroups.end()) return false;
   
   Group lGroup = (*liGroup);
   
   // Check that members of this group are allowed to create files.
   if (!lGroup.permissionsInclude(Group::WritePermissions)) return false;
   
   // Set the file attributes.
   file.creator(user);
   file.owner(user);
   file.group(lGroup);
   file.type(type);
   //file.parent(lParentFile);
   file.path(path);
   
   Date lCreateDate;
   lCreateDate.sample();
   file.createdDate(lCreateDate.sts());
   file.lastModified(lCreateDate.sts());
   
   // On creation, the file is locked by the user that created it.
   WriteLock lWriteLock;
   lWriteLock.user(user);
   lWriteLock.time(l_createDate.sts());
   lWriteLock.lock();
   setWriteLock(file, lWriteLock);
   
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
   
   if (connection().isConnected() && file.isValid())
   {
      l_bSuccess = findAndModify(BSON("_id" << file.object().getObjectID() << 
                                      "wlock.isLocked" << "true"),
                           BSON("$set" << BSON("wlock.isLocked" << "false") <<
                              "$set" << BSON("wlock.user" << mongo::jstNULL)),
                                 false,
                                 true);
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool FileCollection::unlockRead(File &file, User &user)
{
   bool lbSuccess = false;
   
   if (connection().isConnected() && file.isValid())
   {
      l_bSuccess = findAndModify(BSON("_id" << file.object().getObjectID() << 
                                    "rlock.count" << BSON( "$gt" << 0) <<
                                 "rlock.users" << user.object().getObjectID()),
         BSON("$pull" << BSON("rlock.users" << user.object().getObjectID()) <<
                                      "$inc" << BSON("rlock.count" << -1)),
                                 false,
                                 true);
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

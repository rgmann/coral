#include "UserCollection.h"
#include "GroupCollection.h"
#include "FileCollection.h"

//------------------------------------------------------------------------------
UserCollection::UserCollection(MongoConnection &connection)
: Collection(connection, "users")
{
}

//------------------------------------------------------------------------------
bool UserCollection::getGroups(const User &user, std::vector<Group*> &groups)
{
   bool lbSuccess = false;
   mongo::BSONElement lObjectId;
   
   GroupCollection groupCollection(connection());
   std::vector<GenericModel*> lvResults;
   
   if (!user.object().getObjectID(lObjectId)) return false;
   
   lbSuccess = groupCollection.find(BSON("members" << lObjectId),
                                    lvResults);
   
   if (lbSuccess)
   {
      lbSuccess = convert<Group>(groups, lvResults);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
//bool UserCollection::getFiles(const User &user, std::vector<File*> &files)
//{
//   bool lbSuccess = false;
//   mongo::BSONElement lObjectId;
//   Group lGroup;
//   std::string lsCollectionName = connection().dbName() + ".files";
//   
//   std::vector<Group*> lvGroups;
//   std::vector<Group*>::iterator liGroup;
//   
//   if (!getGroups(user, lvGroups)) return false;
//   
//   liGroup = lvGroups.begin();
//   for (; liGroup != lvGroups.end(); liGroup++)
//   {
//      mongo::auto_ptr<mongo::DBClientCursor> lCursor;
//      
//      if ((*liGroup)->object().getObjectID(lObjectId))
//      {
//         lCursor = connection().mongo().query(lsCollectionName,
//                                              BSON("group" << lObjectId));
//         
//         while (lCursor->more())
//         {
//            File* lpFile = new File();
//            lpFile->object(lCursor->next());
//            files.push_back(lpFile);
//         }
//      }
//   }
//   
//   return true;
//}

//------------------------------------------------------------------------------
GenericModel* UserCollection::createModel()
{
   return new User();
}

//------------------------------------------------------------------------------
GenericModel* UserCollection::createModel(const mongo::BSONObj &object)
{
   User* lpUser = new User();
   lpUser->object(object);
   return lpUser;
}

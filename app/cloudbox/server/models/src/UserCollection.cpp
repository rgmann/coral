#include "UserCollection.h"

//------------------------------------------------------------------------------
UserCollection::UserCollection(MongoConnection &connection)
: Collection(connection, "users")
{
}

//------------------------------------------------------------------------------
bool UserCollection::groups(const User &user, std::vector<Group*> &groups)
{
   bool lbSuccess = false;
   mongo::BSONElement lObjectId;
   Group lGroup(m_pDbConn);
   std::vector<GenericModel*> lvResults;
   
   if (!user.object().getObjectID(lObjectId)) return false;
   
   lbSuccess = find(BSON("members" << lObjectId),
                             lvResults);
   
   if (l_bSuccess)
   {
      lbSuccess = convert<Group>(groups, lvResults);
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool UserCollection::files(const User &user, std::vector<File*> &files)
{
   bool lbSuccess = false;
   mongo::BSONElement lObjectId;
   Group lGroup(m_pDbConn);
   std::string lsCollectionName = connection().dbName() + ".files";
   
   std::vector<Group*> lvGroups;
   std::vector<Group*>::iterator liGroup;
   
   if (!groups(user, lvGroups)) return false;
   
   liGroup = lvGroups.begin();
   for (; liGroups != lvGroups.end(); liGroup++) {
      mongo::auto_ptr<DBClientCursor> lCursor;
      
      if (liGroup->object().getObjectId(lObjectId))
      {
         lCursor = connection().mongo().query(lsCollectionName,
                                              BSON("group" << lObjectId),
                                              lvResults);
         
         while (lCursor->more())
         {
            File* lpFile = new File();
            lpFile->object(cursor->next());
            files.push_back(lpFile);
         }
      }
   }
   
   return true;
}

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

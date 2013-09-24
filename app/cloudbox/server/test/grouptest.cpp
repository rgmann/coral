#include <iostream>
#include "MongoConnection.h"
#include "GroupCollection.h"
#include "UserCollection.h"

bool setup(MongoConnection &connection);
bool cleanup(MongoConnection &connection);

void run()
{
   IdList list;
   MongoConnection connection("cbox");
   GroupCollection groups(connection);
   UserCollection  users(connection);
   
   std::cout << "Group Collection Test" << std::endl;

   if (!connection.connect())
   {
      std::cout << "Failed to connect to MongoDB!" << std::endl;
      return;
   }
   
   if (!setup(connection))
   {
      std::cout << "Failed to create groups!" << std::endl;
      return;
   }
   
   
   // Add Robert to Kenneth's group.
   User userRobert;
   User userKenneth;
   users.findOne(BSON("name.first" << "Robert"), userRobert);
   users.findOne(BSON("loginid" << "knoise"), userKenneth);
   if (userRobert.isValid() && userKenneth.isValid())
   {
      Group groupKenneth;
      ObjectId lObjId;
      
      if (userKenneth.getObjectId(lObjId))
      {
         groups.findOne(BSON("owner" << lObjId), groupKenneth);
      
         if (groupKenneth.isValid())
         {
            groups.addMember(groupKenneth, userRobert);
            std::cout << "Added Robert to Kenneth's group." << std::endl;
         }
      }
   }
   
   if (groups.find(mongo::BSONObj(), list))
   {
      std::cout << "Found " << list.size() << " groups" << std::endl;
      IdList::iterator liGroupId = list.begin();
      for (; liGroupId != list.end(); liGroupId++)
      {         
         Group lGroup;
         if (groups.getGroup(*liGroupId, lGroup))
         {
            std::vector<User> members;
            
            std::cout << "Group: " << lGroup.object().toString() << std::endl;
            groups.getMembers(*liGroupId, members);
            
            std::vector<User>::iterator liMember;
            liMember = members.begin();
            for (; liMember != members.end(); liMember++)
            {
               std::cout << "  Member: " << liMember->object().toString()
                         << std::endl;
            }
         }
         else {
            std::cout << "Error getting group: " << liGroupId->toString()
            << std::endl;
         }

      }
   }
   else {
      std::cout << "No groups?" << std::endl;
   }

   cleanup(connection);
}

int main() {
    try {
        run();
    }
   catch( mongo::DBException &e ) {
      std::cout << "caught " << e.what() << std::endl;
    }
    return 0;
}

//------------------------------------------------------------------------------
bool setup(MongoConnection &connection)
{
   int lnCount = 0;
   
   UserCollection users(connection);
   GroupCollection groups(connection);
   User lUser;
   Group lGroup;
   
   if (users.findOne(BSON("name.first" << "Robert"), lUser))
   {
      groups.create(lGroup, lUser);
      std::cout << "Added Robert's group" << std::endl;
      lnCount++;
   }
   
   if (users.findOne(BSON("loginid" << "knoise"), lUser))
   {
      groups.create(lGroup, lUser);
      std::cout << "Added Kenneth's group" << std::endl;
      lnCount++;
   }
   
   if (users.findOne(BSON("loginid" << "nstark"), lUser))
   {
      groups.create(lGroup, lUser);
      std::cout << "Added Ned's group: " << std::endl;
      lnCount++;
   }
   
   return (lnCount == 3);
}

//------------------------------------------------------------------------------
bool cleanup(MongoConnection &connection)
{
   GroupCollection groups(connection);

   return groups.remove(mongo::BSONObj(), false);
}

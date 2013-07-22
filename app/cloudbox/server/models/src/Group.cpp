#include "Group.h"
#include "User.h"

//------------------------------------------------------------------------------
Group::Group()
: GenericModel("groups")
{
}

//------------------------------------------------------------------------------
Group::~Group()
{
}

//------------------------------------------------------------------------------
//bool Group::owner(User& owner)
//{
//   bool l_bSuccess = false;
//   mongo::BSONElement l_userId;
//   std::vector<User> l_vUsers;
//   User l_user(m_pDbConn);
//   
//   if (reload())
//   {
//      l_userId = m_bsonObject.getField("owner");
//      l_bSuccess = l_user.findOne(BSON("_id" << l_userId), owner);
//   }
//   
//   return l_bSuccess;
//}

//------------------------------------------------------------------------------
//bool Group::members(std::vector<User> &members)
//{
//   bool l_bSuccess = false;
//   std::vector<mongo::OID> l_vUserIds;
//   std::vector<mongo::OID>::iterator l_vUserIdsIt;
//   User l_queryUser(m_pDbConn);
//   
//   if (reload())
//   {
//      m_bsonObject.getObjectField("members").Vals(l_vUserIds);
//      
//      l_vUserIdsIt = l_vUserIds.begin();
//      for (; l_vUserIdsIt < l_vUserIds.end(); l_vUserIdsIt++)
//      {
//         User l_user;
//         
//         l_bSuccess = l_queryUser.findOne(BSON("_id" << *l_vUserIdsIt),
//                              l_user);
//         
//         if (l_bSuccess)
//         {
//            members.push_back(l_user);
//         }
//      }
//      
//      l_bSuccess = true;
//   }
//   
//   return l_bSuccess;
//}

#include "Group.h"
#include "User.h"

const char* Group::GroupOwnerField = "owner";
const char* Group::GroupMembersField = "members";
const char* Group::GroupRevField = "revision";
const char* Group::GroupRootField = "root";

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
bool Group::setOwner(const ObjectId& owner)
{
   mongo::BSONObjBuilder lObjBuilder;
      
   lObjBuilder.appendElementsUnique(object());   
   lObjBuilder.append(GroupOwnerField, owner);
   
   object(lObjBuilder.obj());
   
   return true;
}

//------------------------------------------------------------------------------
bool Group::getOwner(ObjectId& owner) const
{
   mongo::BSONElement lOidContainer;
   bool lbSuccess = false;

   if (object().isValid() && object().hasField(GroupOwnerField))
   {
      lOidContainer = object().getField(GroupOwnerField);
      
      if (lOidContainer.type() == mongo::jstOID)
      {
         owner = lOidContainer.OID();
         lbSuccess = true;
      }
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool Group::getMembers(std::vector<ObjectId> &members)
{
   bool lbSuccess = false;
   
   if (object().isValid() && object().hasField(GroupMembersField))
   {
      std::vector<mongo::BSONElement> lvOidContainers;
      std::vector<mongo::BSONElement>::iterator liContainer;
      
      lvOidContainers = object().getField(GroupMembersField).Array();
      
      liContainer = lvOidContainers.begin();
      for (; liContainer != lvOidContainers.end(); liContainer++)
      {
         if (liContainer->type() == mongo::jstOID)
         {
            members.push_back(liContainer->OID());
         }
      }
      
      lbSuccess = true;
   }
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool Group::getRevision(ui32 &revision)
{
   bool lbSuccess = false;
   
   lbSuccess = object().isValid() && object().hasField(GroupRevField);
   
   if (lbSuccess) revision = object().getIntField(GroupRevField);
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
bool Group::setRoot(const ObjectId &root)
{
   mongo::BSONObjBuilder lObjBuilder;
      
   lObjBuilder.appendElementsUnique(object());
   lObjBuilder.append(GroupRootField, root);
   
   object(lObjBuilder.obj());
   
   return true;
}

//------------------------------------------------------------------------------
bool Group::getRoot(ObjectId &root)
{
   bool lbSuccess = false;
      
   if (object().isValid() && object().hasField(GroupRootField))
   {
      mongo::BSONElement lOidContainer;
      
      lOidContainer = object().getField(GroupRootField);
      
      if (lOidContainer.type() == mongo::jstOID)
      {
         root = lOidContainer.OID();
         lbSuccess = true;
      }
   }
   
   return lbSuccess;
}

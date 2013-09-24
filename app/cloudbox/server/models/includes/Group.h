#ifndef GROUP_H
#define GROUP_H

#include <string>
#include "GenericModel.h"

class User;
class File;

class Group : public GenericModel
{
public:
   
   static const char* GroupMembersField;
   static const char* GroupOwnerField;
   static const char* GroupRevField;
   static const char* GroupRootField;
      
   Group();
   
   ~Group();
   
   bool setOwner(const ObjectId &owner);
   
   /**
    * Creates an incomplete user with the user ID populated.
    * The caller must then query on the user.
    */
   bool getOwner(ObjectId &owner) const;
   
   /**
    * @note Members are added via the collection.
    */
   bool getMembers(std::vector<ObjectId> &members);

   bool getRevision(ui32 &revision);
   
   bool setRoot(const ObjectId &root);
   
   bool getRoot(ObjectId &root);

protected:
   
};

#endif // GROUP_H
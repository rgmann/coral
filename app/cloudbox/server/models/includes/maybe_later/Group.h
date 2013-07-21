#ifndef GROUP_H
#define GROUP_H

#include <string>
#include "GenericModel.h"

class User;
//class File;

class Group : public GenericModel
{
public:
      
   explicit Group(MongoConnection* pConnection = NULL);
   
   ~Group();
   
   bool owner(User &owner);
   
   bool members(std::vector<User> &members);

   bool addMember(const User& user);

   ui32 revision();
   
   /**
    * Atomic increment of group revision number.
    */
   //bool  addRevision();

   //File& root() const;
protected:
   
   GenericModel* create(MongoConnection* pConnection) const;
};

#endif // GROUP_H
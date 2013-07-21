#ifndef GROUP_H
#define GROUP_H

#include <string>
#include "GenericModel.h"

class User;
class File;

class Group : public GenericModel
{
public:
      
   Group();
   
   ~Group();
   
   bool owner(User &owner);

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
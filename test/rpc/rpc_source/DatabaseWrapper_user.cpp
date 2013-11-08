#include "DatabaseWrapper.h"

//-----------------------------------------------------------------------------
bool DatabaseWrapper::
create(std::string lastname, const UserAttributes& attrs)
{
   /* TODO: Implement action here. */
   bool lReturnValue = mpInst->create(lastname, attrs);

   return lReturnValue;
}


//-----------------------------------------------------------------------------
bool DatabaseWrapper::
read(std::string lastname, UserAttributes& attrs)
{
   /* TODO: Implement action here. */
   bool lReturnValue = mpInst->read(lastname, attrs);

   return lReturnValue;
}


//-----------------------------------------------------------------------------
bool DatabaseWrapper::
update(std::string lastname, const UserAttributes& attrs)
{
   /* TODO: Implement action here. */
   bool lReturnValue = mpInst->update(lastname, attrs);

   return lReturnValue;
}


//-----------------------------------------------------------------------------
bool DatabaseWrapper::
destroy(std::string lastname)
{
   /* TODO: Implement action here. */
   bool lReturnValue = mpInst->destroy(lastname);

   return lReturnValue;
}




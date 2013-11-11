#include "DatabaseCreateInput.h"

//-----------------------------------------------------------------------------
DatabaseCreateInput::DatabaseCreateInput() {}

//-----------------------------------------------------------------------------
DatabaseCreateInput::DatabaseCreateInput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void DatabaseCreateInput::marshall(std::string lastname, const UserAttributes& attrs)
{
   myParameters.set("lastname", lastname);
   myParameters.set("attrs", attrs);
}

//-----------------------------------------------------------------------------
bool DatabaseCreateInput::unmarshall(std::string& lastname, UserAttributes& attrs)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("lastname", lastname);
   lbSuccess &= myParameters.get("attrs", attrs);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool DatabaseCreateInput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("lastname");
   lbValid &= myParameters.hasField("attrs");

   return lbValid;
}



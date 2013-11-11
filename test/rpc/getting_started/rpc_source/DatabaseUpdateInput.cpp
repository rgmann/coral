#include "DatabaseUpdateInput.h"

//-----------------------------------------------------------------------------
DatabaseUpdateInput::DatabaseUpdateInput() {}

//-----------------------------------------------------------------------------
DatabaseUpdateInput::DatabaseUpdateInput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void DatabaseUpdateInput::marshall(std::string lastname, const UserAttributes& attrs)
{
   myParameters.set("lastname", lastname);
   myParameters.set("attrs", attrs);
}

//-----------------------------------------------------------------------------
bool DatabaseUpdateInput::unmarshall(std::string& lastname, UserAttributes& attrs)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("lastname", lastname);
   lbSuccess &= myParameters.get("attrs", attrs);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool DatabaseUpdateInput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("lastname");
   lbValid &= myParameters.hasField("attrs");

   return lbValid;
}



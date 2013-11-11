#include "DatabaseDestroyInput.h"

//-----------------------------------------------------------------------------
DatabaseDestroyInput::DatabaseDestroyInput() {}

//-----------------------------------------------------------------------------
DatabaseDestroyInput::DatabaseDestroyInput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void DatabaseDestroyInput::marshall(std::string lastname)
{
   myParameters.set("lastname", lastname);
}

//-----------------------------------------------------------------------------
bool DatabaseDestroyInput::unmarshall(std::string& lastname)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("lastname", lastname);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool DatabaseDestroyInput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("lastname");

   return lbValid;
}



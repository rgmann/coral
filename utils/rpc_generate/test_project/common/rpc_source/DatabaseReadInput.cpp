#include "DatabaseReadInput.h"

//-----------------------------------------------------------------------------
DatabaseReadInput::DatabaseReadInput() {}

//-----------------------------------------------------------------------------
DatabaseReadInput::DatabaseReadInput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void DatabaseReadInput::marshall(std::string lastname, UserAttributes& attrs)
{
   myParameters.set("lastname", lastname);
   myParameters.set("attrs", attrs);
}

//-----------------------------------------------------------------------------
bool DatabaseReadInput::unmarshall(std::string& lastname, UserAttributes& attrs)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("lastname", lastname);
   lbSuccess &= myParameters.get("attrs", attrs);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool DatabaseReadInput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("lastname");
   lbValid &= myParameters.hasField("attrs");

   return lbValid;
}



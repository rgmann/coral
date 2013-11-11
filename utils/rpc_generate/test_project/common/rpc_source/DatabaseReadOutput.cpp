#include "DatabaseReadOutput.h"

//-----------------------------------------------------------------------------
DatabaseReadOutput::DatabaseReadOutput() {}

//-----------------------------------------------------------------------------
DatabaseReadOutput::DatabaseReadOutput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void DatabaseReadOutput::marshall(UserAttributes& attrs)
{
   myParameters.set("attrs", attrs);
}

//-----------------------------------------------------------------------------
bool DatabaseReadOutput::unmarshall(UserAttributes& attrs)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("attrs", attrs);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool DatabaseReadOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("attrs");
   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void DatabaseReadOutput::setReturnValue(bool returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool DatabaseReadOutput::getReturnValue(bool& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




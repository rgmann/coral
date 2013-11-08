#include "DatabaseUpdateOutput.h"

//-----------------------------------------------------------------------------
DatabaseUpdateOutput::DatabaseUpdateOutput() {}

//-----------------------------------------------------------------------------
DatabaseUpdateOutput::DatabaseUpdateOutput(const Structure& params)
: RpcParameterList(params)
{
}


//-----------------------------------------------------------------------------
bool DatabaseUpdateOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void DatabaseUpdateOutput::setReturnValue(bool returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool DatabaseUpdateOutput::getReturnValue(bool& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




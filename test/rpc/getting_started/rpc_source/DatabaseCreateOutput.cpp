#include "DatabaseCreateOutput.h"

//-----------------------------------------------------------------------------
DatabaseCreateOutput::DatabaseCreateOutput() {}

//-----------------------------------------------------------------------------
DatabaseCreateOutput::DatabaseCreateOutput(const Structure& params)
: RpcParameterList(params)
{
}


//-----------------------------------------------------------------------------
bool DatabaseCreateOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void DatabaseCreateOutput::setReturnValue(bool returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool DatabaseCreateOutput::getReturnValue(bool& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




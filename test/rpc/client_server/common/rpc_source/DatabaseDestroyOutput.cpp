#include "DatabaseDestroyOutput.h"

//-----------------------------------------------------------------------------
DatabaseDestroyOutput::DatabaseDestroyOutput() {}

//-----------------------------------------------------------------------------
DatabaseDestroyOutput::DatabaseDestroyOutput(const Structure& params)
: RpcParameterList(params)
{
}


//-----------------------------------------------------------------------------
bool DatabaseDestroyOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void DatabaseDestroyOutput::setReturnValue(bool returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool DatabaseDestroyOutput::getReturnValue(bool& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




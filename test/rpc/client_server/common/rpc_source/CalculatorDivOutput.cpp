#include "CalculatorDivOutput.h"

//-----------------------------------------------------------------------------
CalculatorDivOutput::CalculatorDivOutput() {}

//-----------------------------------------------------------------------------
CalculatorDivOutput::CalculatorDivOutput(const Structure& params)
: RpcParameterList(params)
{
}


//-----------------------------------------------------------------------------
bool CalculatorDivOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void CalculatorDivOutput::setReturnValue(i32 returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool CalculatorDivOutput::getReturnValue(i32& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




#include "CalculatorGetnumopsOutput.h"

//-----------------------------------------------------------------------------
CalculatorGetnumopsOutput::CalculatorGetnumopsOutput() {}

//-----------------------------------------------------------------------------
CalculatorGetnumopsOutput::CalculatorGetnumopsOutput(const Structure& params)
: RpcParameterList(params)
{
}


//-----------------------------------------------------------------------------
bool CalculatorGetnumopsOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void CalculatorGetnumopsOutput::setReturnValue(i32 returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool CalculatorGetnumopsOutput::getReturnValue(i32& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




#include "CalculatorMulOutput.h"

//-----------------------------------------------------------------------------
CalculatorMulOutput::CalculatorMulOutput() {}

//-----------------------------------------------------------------------------
CalculatorMulOutput::CalculatorMulOutput(const Structure& params)
: RpcParameterList(params)
{
}


//-----------------------------------------------------------------------------
bool CalculatorMulOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void CalculatorMulOutput::setReturnValue(i32 returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool CalculatorMulOutput::getReturnValue(i32& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




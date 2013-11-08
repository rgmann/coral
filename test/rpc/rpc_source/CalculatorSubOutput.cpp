#include "CalculatorSubOutput.h"

//-----------------------------------------------------------------------------
CalculatorSubOutput::CalculatorSubOutput() {}

//-----------------------------------------------------------------------------
CalculatorSubOutput::CalculatorSubOutput(const Structure& params)
: RpcParameterList(params)
{
}


//-----------------------------------------------------------------------------
bool CalculatorSubOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void CalculatorSubOutput::setReturnValue(i32 returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool CalculatorSubOutput::getReturnValue(i32& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




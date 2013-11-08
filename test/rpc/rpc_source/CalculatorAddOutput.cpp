#include "CalculatorAddOutput.h"

//-----------------------------------------------------------------------------
CalculatorAddOutput::CalculatorAddOutput() {}

//-----------------------------------------------------------------------------
CalculatorAddOutput::CalculatorAddOutput(const Structure& params)
: RpcParameterList(params)
{
}


//-----------------------------------------------------------------------------
bool CalculatorAddOutput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField(RpcReturnValue);

   return lbValid;
}

//-----------------------------------------------------------------------------
void CalculatorAddOutput::setReturnValue(i32 returnValue)
{
   myParameters.set(RpcReturnValue, returnValue);
}

//-----------------------------------------------------------------------------
bool CalculatorAddOutput::getReturnValue(i32& returnValue) const
{
   bool lbSuccess = true;

   lbSuccess = myParameters.get(RpcReturnValue, returnValue);

   return lbSuccess;
}




#include "CalculatorMulInput.h"

//-----------------------------------------------------------------------------
CalculatorMulInput::CalculatorMulInput() {}

//-----------------------------------------------------------------------------
CalculatorMulInput::CalculatorMulInput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void CalculatorMulInput::marshall(i32 a, i32 b)
{
   myParameters.set("a", a);
   myParameters.set("b", b);
}

//-----------------------------------------------------------------------------
bool CalculatorMulInput::unmarshall(i32& a, i32& b)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("a", a);
   lbSuccess &= myParameters.get("b", b);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool CalculatorMulInput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("a");
   lbValid &= myParameters.hasField("b");

   return lbValid;
}



#include "CalculatorDivInput.h"

//-----------------------------------------------------------------------------
CalculatorDivInput::CalculatorDivInput() {}

//-----------------------------------------------------------------------------
CalculatorDivInput::CalculatorDivInput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void CalculatorDivInput::marshall(i32 a, i32 b)
{
   myParameters.set("a", a);
   myParameters.set("b", b);
}

//-----------------------------------------------------------------------------
bool CalculatorDivInput::unmarshall(i32& a, i32& b)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("a", a);
   lbSuccess &= myParameters.get("b", b);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool CalculatorDivInput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("a");
   lbValid &= myParameters.hasField("b");

   return lbValid;
}



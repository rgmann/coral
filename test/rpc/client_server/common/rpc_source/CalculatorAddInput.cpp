#include "CalculatorAddInput.h"

//-----------------------------------------------------------------------------
CalculatorAddInput::CalculatorAddInput() {}

//-----------------------------------------------------------------------------
CalculatorAddInput::CalculatorAddInput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void CalculatorAddInput::marshall(i32 a, i32 b)
{
   myParameters.set("a", a);
   myParameters.set("b", b);
}

//-----------------------------------------------------------------------------
bool CalculatorAddInput::unmarshall(i32& a, i32& b)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("a", a);
   lbSuccess &= myParameters.get("b", b);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool CalculatorAddInput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("a");
   lbValid &= myParameters.hasField("b");

   return lbValid;
}



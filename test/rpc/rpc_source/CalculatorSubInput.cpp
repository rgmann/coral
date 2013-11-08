#include "CalculatorSubInput.h"

//-----------------------------------------------------------------------------
CalculatorSubInput::CalculatorSubInput() {}

//-----------------------------------------------------------------------------
CalculatorSubInput::CalculatorSubInput(const Structure& params)
: RpcParameterList(params)
{
}

//-----------------------------------------------------------------------------
void CalculatorSubInput::marshall(i32 a, i32 b)
{
   myParameters.set("a", a);
   myParameters.set("b", b);
}

//-----------------------------------------------------------------------------
bool CalculatorSubInput::unmarshall(i32& a, i32& b)
{
   bool lbSuccess = true;

   lbSuccess &= myParameters.get("a", a);
   lbSuccess &= myParameters.get("b", b);

   return lbSuccess;
}



//-----------------------------------------------------------------------------
bool CalculatorSubInput::isValid() const
{
   bool lbValid = true;

   lbValid &= myParameters.hasField("a");
   lbValid &= myParameters.hasField("b");

   return lbValid;
}



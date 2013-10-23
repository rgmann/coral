#include "CalculatorDivParamList.h"

//-----------------------------------------------------------------------------
CalculatorDivParamList::
CalculatorDivParamList(i32 a, i32 b)
{
   Structure::set("a", a);
   Structure::set("b", b);
}

//-----------------------------------------------------------------------------
CalculatorDivParamList::
~CalculatorDivParamList()
{
}

//-----------------------------------------------------------------------------
bool CalculatorDivParamList::valid() const
{
   bool lbIsValid = true;

   lbIsValid &= Structure::hasField("a");
   lbIsValid &= Structure::hasField("b");

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool CalculatorDivParamList::get(i32& a, i32& b)
{
   bool lbIsValid = true;

   lbIsValid &= Structure::get("a", a);
   lbIsValid &= Structure::get("b", b);

   return lbIsValid;
}


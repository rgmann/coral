#include "CalculatorSubParamList.h"

//-----------------------------------------------------------------------------
CalculatorSubParamList::
CalculatorSubParamList(i32 a, i32 b)
{
   Structure::set("a", a);
   Structure::set("b", b);
}

//-----------------------------------------------------------------------------
CalculatorSubParamList::
~CalculatorSubParamList()
{
}

//-----------------------------------------------------------------------------
bool CalculatorSubParamList::valid() const
{
   bool lbIsValid = true;

   lbIsValid &= Structure::hasField("a");
   lbIsValid &= Structure::hasField("b");

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool CalculatorSubParamList::get(i32& a, i32& b)
{
   bool lbIsValid = true;

   lbIsValid &= Structure::get("a", a);
   lbIsValid &= Structure::get("b", b);

   return lbIsValid;
}


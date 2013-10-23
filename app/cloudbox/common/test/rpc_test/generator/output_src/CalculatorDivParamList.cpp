#include "CalculatorDivParamList.h"

//-----------------------------------------------------------------------------
CalculatorDivParamList::
CalculatorDivParamList(i32 a, i32 b)
{
   set("a", a);
   set("b", b);
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

   lbIsValid &= hasField(a);
   lbIsValid &= hasField(b);

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool CalculatorDivParamList::get(i32& a, i32& b)
{
   bool lbIsValid = true;

   lbIsValid &= get("a", a);
   lbIsValid &= get("b", b);

   return lbIsValid;
}


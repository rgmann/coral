#include "CalculatorSubParamList.h"

//-----------------------------------------------------------------------------
CalculatorSubParamList::
CalculatorSubParamList(i32 a, i32 b)
{
   set("a", a);
   set("b", b);
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

   lbIsValid &= hasField(a);
   lbIsValid &= hasField(b);

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool CalculatorSubParamList::get(i32& a, i32& b)
{
   bool lbIsValid = true;

   lbIsValid &= get("a", a);
   lbIsValid &= get("b", b);

   return lbIsValid;
}


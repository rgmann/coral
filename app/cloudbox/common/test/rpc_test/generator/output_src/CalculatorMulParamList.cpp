#include "CalculatorMulParamList.h"

//-----------------------------------------------------------------------------
CalculatorMulParamList::
CalculatorMulParamList(i32 a, i32 b)
{
   set("a", a);
   set("b", b);
}

//-----------------------------------------------------------------------------
CalculatorMulParamList::
~CalculatorMulParamList()
{
}

//-----------------------------------------------------------------------------
bool CalculatorMulParamList::valid() const
{
   bool lbIsValid = true;

   lbIsValid &= hasField(a);
   lbIsValid &= hasField(b);

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool CalculatorMulParamList::get(i32& a, i32& b)
{
   bool lbIsValid = true;

   lbIsValid &= get("a", a);
   lbIsValid &= get("b", b);

   return lbIsValid;
}


#include "CalculatorAddParamList.h"

//-----------------------------------------------------------------------------
CalculatorAddParamList::
CalculatorAddParamList(i32 a, i32 b)
{
   set("a", a);
   set("b", b);
}

//-----------------------------------------------------------------------------
CalculatorAddParamList::
~CalculatorAddParamList()
{
}

//-----------------------------------------------------------------------------
bool CalculatorAddParamList::valid() const
{
   bool lbIsValid = true;

   lbIsValid &= hasField(a);
   lbIsValid &= hasField(b);

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool CalculatorAddParamList::get(i32& a, i32& b)
{
   bool lbIsValid = true;

   lbIsValid &= get("a", a);
   lbIsValid &= get("b", b);

   return lbIsValid;
}


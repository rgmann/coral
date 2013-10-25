#include "CalculatorsubParamList.h"

//-----------------------------------------------------------------------------
CalculatorsubParamList::
CalculatorsubParamListParamList(i32 a, i32 b)
{
   Structure::set("a", a);
   Structure::set("b", b);
}

//-----------------------------------------------------------------------------
CalculatorsubParamList::
~CalculatorsubParamListParamList()
{
}

//-----------------------------------------------------------------------------
bool CalculatorsubParamListParamList::valid() const
{
   bool lbIsValid = true;

   lbIsValid &= Structure::hasField("a");
   lbIsValid &= Structure::hasField("b");

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool CalculatorsubParamListParamList::get(i32& a, i32& b)
{
   bool lbIsValid = true;

   lbIsValid &= Structure::get("a", a);
   lbIsValid &= Structure::get("b", b);

   return lbIsValid;
}


#include "CalculatordivParamList.h"

//-----------------------------------------------------------------------------
CalculatordivParamList::
CalculatordivParamListParamList(i32 a, i32 b)
{
   Structure::set("a", a);
   Structure::set("b", b);
}

//-----------------------------------------------------------------------------
CalculatordivParamList::
~CalculatordivParamListParamList()
{
}

//-----------------------------------------------------------------------------
bool CalculatordivParamListParamList::valid() const
{
   bool lbIsValid = true;

   lbIsValid &= Structure::hasField("a");
   lbIsValid &= Structure::hasField("b");

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool CalculatordivParamListParamList::get(i32& a, i32& b)
{
   bool lbIsValid = true;

   lbIsValid &= Structure::get("a", a);
   lbIsValid &= Structure::get("b", b);

   return lbIsValid;
}


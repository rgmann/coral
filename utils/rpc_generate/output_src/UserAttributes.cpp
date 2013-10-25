#include "UserAttributes.h"

//-----------------------------------------------------------------------------
UserAttributes::
UserAttributesParamList(string first_name, i32 age, string eye_color)
{
   Structure::set("first_name", first_name);
   Structure::set("age", age);
   Structure::set("eye_color", eye_color);
}

//-----------------------------------------------------------------------------
UserAttributes::
~UserAttributesParamList()
{
}

//-----------------------------------------------------------------------------
bool UserAttributesParamList::valid() const
{
   bool lbIsValid = true;

   lbIsValid &= Structure::hasField("first_name");
   lbIsValid &= Structure::hasField("age");
   lbIsValid &= Structure::hasField("eye_color");

   return lbIsValid;
}

//-----------------------------------------------------------------------------
bool UserAttributesParamList::get(string& first_name, i32& age, string& eye_color)
{
   bool lbIsValid = true;

   lbIsValid &= Structure::get("first_name", first_name);
   lbIsValid &= Structure::get("age", age);
   lbIsValid &= Structure::get("eye_color", eye_color);

   return lbIsValid;
}


#ifndef USER_ATTRIBUTES_H
#define USER_ATTRIBUTES_H

#include "Structure.h"

class UserAttributes : public Structure {
public:

   UserAttributes(string first_name, i32 age, string eye_color);
   ~UserAttributes();

   bool valid() const;

   bool get(string& first_name, i32& age, string& eye_color);
};

#endif // USER_ATTRIBUTES_H

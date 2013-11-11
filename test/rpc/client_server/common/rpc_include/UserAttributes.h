#ifndef USER_ATTRIBUTES_H
#define USER_ATTRIBUTES_H

#include "Structure.h"


class UserAttributes : public Structure {
public:

   UserAttributes();
   UserAttributes(std::string first_name, i32 age, std::string eye_color);
   ~UserAttributes();

   bool valid() const;

   bool get(std::string& first_name, i32& age, std::string& eye_color);
};

#endif // $$COMPILER_GUARD$$_H

#ifndef CALCULATORMUL_PARAM_LIST_H
#define CALCULATORMUL_PARAM_LIST_H

#include "Structure.h"

class CalculatormulParamList : public Structure {
public:

   CalculatormulParamList(i32 a, i32 b);
   ~CalculatormulParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATORMUL_PARAM_LIST_H

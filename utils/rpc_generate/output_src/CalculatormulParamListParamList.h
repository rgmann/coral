#ifndef CALCULATOR_MUL_PARAM_LIST_H
#define CALCULATOR_MUL_PARAM_LIST_H

#include "Structure.h"

class CalculatormulParamListParamList : public Structure {
public:

   CalculatormulParamListParamList(i32 a, i32 b);
   ~CalculatormulParamListParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATOR_MUL_PARAM_LIST_H

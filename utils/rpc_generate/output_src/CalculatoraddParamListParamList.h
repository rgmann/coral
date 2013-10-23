#ifndef CALCULATOR_ADD_PARAM_LIST_H
#define CALCULATOR_ADD_PARAM_LIST_H

#include "Structure.h"

class CalculatoraddParamListParamList : public Structure {
public:

   CalculatoraddParamListParamList(i32 a, i32 b);
   ~CalculatoraddParamListParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATOR_ADD_PARAM_LIST_H

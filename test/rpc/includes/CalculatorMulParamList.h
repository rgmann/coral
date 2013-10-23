#ifndef CALCULATOR_MUL_PARAM_LIST_H
#define CALCULATOR_MUL_PARAM_LIST_H

#include "Structure.h"

class CalculatorMulParamList : public Structure {
public:

   CalculatorMulParamList(i32 a, i32 b);
   ~CalculatorMulParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATOR_MUL_PARAM_LIST_H

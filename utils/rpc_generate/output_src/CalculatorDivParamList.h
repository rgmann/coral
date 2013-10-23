#ifndef CALCULATOR_DIV_PARAM_LIST_H
#define CALCULATOR_DIV_PARAM_LIST_H

#include "Structure.h"

class CalculatorDivParamList : public Structure {
public:

   CalculatorDivParamList(i32 a, i32 b);
   ~CalculatorDivParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATOR_DIV_PARAM_LIST_H

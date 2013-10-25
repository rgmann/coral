#ifndef CALCULATORADD_PARAM_LIST_H
#define CALCULATORADD_PARAM_LIST_H

#include "Structure.h"

class CalculatoraddParamList : public Structure {
public:

   CalculatoraddParamList(i32 a, i32 b);
   ~CalculatoraddParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATORADD_PARAM_LIST_H

#ifndef CALCULATOR_SUB_PARAM_LIST_H
#define CALCULATOR_SUB_PARAM_LIST_H

#include "Structure.h"

class CalculatorsubParamListParamList : public Structure {
public:

   CalculatorsubParamListParamList(i32 a, i32 b);
   ~CalculatorsubParamListParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATOR_SUB_PARAM_LIST_H

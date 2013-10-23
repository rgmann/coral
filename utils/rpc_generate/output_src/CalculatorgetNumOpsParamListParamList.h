#ifndef CALCULATOR_GETNUMOPS_PARAM_LIST_H
#define CALCULATOR_GETNUMOPS_PARAM_LIST_H

#include "Structure.h"

class CalculatorgetNumOpsParamListParamList : public Structure {
public:

   CalculatorgetNumOpsParamListParamList();
   ~CalculatorgetNumOpsParamListParamList();

   bool valid() const;

   bool get();
};

#endif // CALCULATOR_GETNUMOPS_PARAM_LIST_H

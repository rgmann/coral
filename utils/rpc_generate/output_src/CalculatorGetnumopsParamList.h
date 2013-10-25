#ifndef CALCULATORGET_NUM_OPS_PARAM_LIST_H
#define CALCULATORGET_NUM_OPS_PARAM_LIST_H

#include "Structure.h"

class CalculatorgetNumOpsParamList : public Structure {
public:

   CalculatorgetNumOpsParamList();
   ~CalculatorgetNumOpsParamList();

   bool valid() const;

   bool get();
};

#endif // CALCULATORGET_NUM_OPS_PARAM_LIST_H

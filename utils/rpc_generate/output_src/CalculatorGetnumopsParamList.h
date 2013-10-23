#ifndef CALCULATOR_GETNUMOPS_PARAM_LIST_H
#define CALCULATOR_GETNUMOPS_PARAM_LIST_H

#include "Structure.h"

class CalculatorGetnumopsParamList : public Structure {
public:

   CalculatorGetnumopsParamList();
   ~CalculatorGetnumopsParamList();

   bool valid() const;

   bool get();
};

#endif // CALCULATOR_GETNUMOPS_PARAM_LIST_H

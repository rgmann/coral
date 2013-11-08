#ifndef CALCULATOR_GETNUMOPS_INPUT_H
#define CALCULATOR_GETNUMOPS_INPUT_H

#include "RpcParameterList.h"

class CalculatorGetnumopsInput : public RpcParameterList {
public:

   CalculatorGetnumopsInput();
   CalculatorGetnumopsInput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

};

#endif // CALCULATOR_GETNUMOPS_INPUT_H

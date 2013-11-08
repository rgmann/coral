#ifndef CALCULATOR_GETNUMOPS_OUTPUT_H
#define CALCULATOR_GETNUMOPS_OUTPUT_H

#include "RpcParameterList.h"

class CalculatorGetnumopsOutput : public RpcParameterList {
public:

   CalculatorGetnumopsOutput();
   CalculatorGetnumopsOutput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

   void setReturnValue(i32 returnValue);
   bool getReturnValue(i32& returnValue) const;
};

#endif // CALCULATOR_GETNUMOPS_OUTPUT_H

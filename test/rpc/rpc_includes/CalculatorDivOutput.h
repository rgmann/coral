#ifndef CALCULATOR_DIV_OUTPUT_H
#define CALCULATOR_DIV_OUTPUT_H

#include "RpcParameterList.h"

class CalculatorDivOutput : public RpcParameterList {
public:

   CalculatorDivOutput();
   CalculatorDivOutput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

   void setReturnValue(i32 returnValue);
   bool getReturnValue(i32& returnValue) const;
};

#endif // CALCULATOR_DIV_OUTPUT_H

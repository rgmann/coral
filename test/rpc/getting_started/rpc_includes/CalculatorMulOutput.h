#ifndef CALCULATOR_MUL_OUTPUT_H
#define CALCULATOR_MUL_OUTPUT_H

#include "RpcParameterList.h"

class CalculatorMulOutput : public RpcParameterList {
public:

   CalculatorMulOutput();
   CalculatorMulOutput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

   void setReturnValue(i32 returnValue);
   bool getReturnValue(i32& returnValue) const;
};

#endif // CALCULATOR_MUL_OUTPUT_H

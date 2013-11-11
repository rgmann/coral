#ifndef CALCULATOR_SUB_OUTPUT_H
#define CALCULATOR_SUB_OUTPUT_H

#include "RpcParameterList.h"

class CalculatorSubOutput : public RpcParameterList {
public:

   CalculatorSubOutput();
   CalculatorSubOutput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

   void setReturnValue(i32 returnValue);
   bool getReturnValue(i32& returnValue) const;
};

#endif // CALCULATOR_SUB_OUTPUT_H

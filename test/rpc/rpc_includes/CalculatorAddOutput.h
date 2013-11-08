#ifndef CALCULATOR_ADD_OUTPUT_H
#define CALCULATOR_ADD_OUTPUT_H

#include "RpcParameterList.h"

class CalculatorAddOutput : public RpcParameterList {
public:

   CalculatorAddOutput();
   CalculatorAddOutput(const Structure& params);

   using RpcParameterList::marshall;

   using RpcParameterList::unmarshall;

   bool isValid() const;

   void setReturnValue(i32 returnValue);
   bool getReturnValue(i32& returnValue) const;
};

#endif // CALCULATOR_ADD_OUTPUT_H

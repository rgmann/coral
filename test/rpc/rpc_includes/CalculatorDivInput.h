#ifndef CALCULATOR_DIV_INPUT_H
#define CALCULATOR_DIV_INPUT_H

#include "RpcParameterList.h"

class CalculatorDivInput : public RpcParameterList {
public:

   CalculatorDivInput();
   CalculatorDivInput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(i32 a, i32 b);

   using RpcParameterList::unmarshall;
   bool unmarshall(i32& a, i32& b);

   bool isValid() const;

};

#endif // CALCULATOR_DIV_INPUT_H

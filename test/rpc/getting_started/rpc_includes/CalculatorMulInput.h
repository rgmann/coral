#ifndef CALCULATOR_MUL_INPUT_H
#define CALCULATOR_MUL_INPUT_H

#include "RpcParameterList.h"

class CalculatorMulInput : public RpcParameterList {
public:

   CalculatorMulInput();
   CalculatorMulInput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(i32 a, i32 b);

   using RpcParameterList::unmarshall;
   bool unmarshall(i32& a, i32& b);

   bool isValid() const;

};

#endif // CALCULATOR_MUL_INPUT_H

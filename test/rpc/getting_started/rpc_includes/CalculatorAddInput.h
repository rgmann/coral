#ifndef CALCULATOR_ADD_INPUT_H
#define CALCULATOR_ADD_INPUT_H

#include "RpcParameterList.h"

class CalculatorAddInput : public RpcParameterList {
public:

   CalculatorAddInput();
   CalculatorAddInput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(i32 a, i32 b);

   using RpcParameterList::unmarshall;
   bool unmarshall(i32& a, i32& b);

   bool isValid() const;

};

#endif // CALCULATOR_ADD_INPUT_H

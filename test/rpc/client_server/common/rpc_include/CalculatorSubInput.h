#ifndef CALCULATOR_SUB_INPUT_H
#define CALCULATOR_SUB_INPUT_H

#include "RpcParameterList.h"

class CalculatorSubInput : public RpcParameterList {
public:

   CalculatorSubInput();
   CalculatorSubInput(const Structure& params);

   using RpcParameterList::marshall;
   void marshall(i32 a, i32 b);

   using RpcParameterList::unmarshall;
   bool unmarshall(i32& a, i32& b);

   bool isValid() const;

};

#endif // CALCULATOR_SUB_INPUT_H

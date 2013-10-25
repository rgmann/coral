#ifndef CALCULATORSUB_PARAM_LIST_H
#define CALCULATORSUB_PARAM_LIST_H

#include "Structure.h"

class CalculatorsubParamList : public Structure {
public:

   CalculatorsubParamList(i32 a, i32 b);
   ~CalculatorsubParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATORSUB_PARAM_LIST_H

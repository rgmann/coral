#ifndef CALCULATOR_DIV_PARAM_LIST_H
#define CALCULATOR_DIV_PARAM_LIST_H

#include "Structure.h"

class CalculatordivParamListParamList : public Structure {
public:

   CalculatordivParamListParamList(i32 a, i32 b);
   ~CalculatordivParamListParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATOR_DIV_PARAM_LIST_H

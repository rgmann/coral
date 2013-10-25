#ifndef CALCULATORDIV_PARAM_LIST_H
#define CALCULATORDIV_PARAM_LIST_H

#include "Structure.h"

class CalculatordivParamList : public Structure {
public:

   CalculatordivParamList(i32 a, i32 b);
   ~CalculatordivParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATORDIV_PARAM_LIST_H

#ifndef CALCULATOR_SUB_PARAM_LIST_H
#define CALCULATOR_SUB_PARAM_LIST_H

class CalculatorSubParamList : public Structure {
public:

   CalculatorSubParamList(i32 a, i32 b);
   ~CalculatorSubParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATOR_SUB_PARAM_LIST_H

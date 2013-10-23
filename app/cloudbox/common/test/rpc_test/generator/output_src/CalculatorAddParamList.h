#ifndef CALCULATOR_ADD_PARAM_LIST_H
#define CALCULATOR_ADD_PARAM_LIST_H

class CalculatorAddParamList : public Structure {
public:

   CalculatorAddParamList(i32 a, i32 b);
   ~CalculatorAddParamList();

   bool valid() const;

   bool get(i32& a, i32& b);
};

#endif // CALCULATOR_ADD_PARAM_LIST_H

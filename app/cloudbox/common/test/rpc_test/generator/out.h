#ifndef CALCULATOR_WRAPPER_H
#define CALCULATOR_WRAPPER_H

class CalculatorWrapper : public InstanceWrapper {
public:

   Calculator(const Structure &params);
   ~Calculator(const Structure &params);

   void add(InstanceWrapper* pInst, Structure &params, RpcReturnValue &retval);
   void sub(InstanceWrapper* pInst, Structure &params, RpcReturnValue &retval);

private:

   Calculator* mpInst;
};
#endif // CALCULATOR_WRAPPER_H

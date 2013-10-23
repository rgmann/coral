#ifndef CALCULATOR_WRAPPER_H
#define CALCULATOR_WRAPPER_H

#include "InstanceWrapper.h"
#include "Calculator.h"

class CalculatorWrapper : public InstanceWrapper {
public:

   CalculatorWrapper();
   ~CalculatorWrapper();

   bool initialize(const Structure& params);
   bool destroy(const Structure& params);

   static void add(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);
   static void sub(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);
   static void mul(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);
   static void div(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);
   static void getNumOps(InstanceWrapper *pInst, Structure &params, RpcReturnValue &retobj);

private:

   Calculator* mpInst;
};
#endif // CALCULATOR_WRAPPER_H

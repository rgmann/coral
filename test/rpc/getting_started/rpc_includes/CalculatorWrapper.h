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

   static RpcException addDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);
   static RpcException subDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);
   static RpcException mulDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);
   static RpcException divDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);
   static RpcException getNumOpsDelegate(InstanceWrapper *pInst, const Structure& input, Structure& output);

private:

   i32 add(i32 a, i32 b);
   i32 sub(i32 a, i32 b);
   i32 mul(i32 a, i32 b);
   i32 div(i32 a, i32 b);
   i32 getNumOps();

private:

   Calculator* mpInst;
};

#endif // CALCULATOR_WRAPPER_H

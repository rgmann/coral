#ifndef CALCULATOR_WRAPPER_H
#define CALCULATOR_WRAPPER_H

#include "InstanceWrapper.h"
#include "Calculator.h"

using namespace liber::rpc;

class CalculatorWrapper : public liber::rpc::InstanceWrapper {
public:

   CalculatorWrapper();
   ~CalculatorWrapper();

   bool initialize(const std::string& params);
   bool destroy(const std::string& params);

   static RpcException addDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output);
   static RpcException subDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output);
   static RpcException mulDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output);
   static RpcException divDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output);
   static RpcException getNumOpsDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output);

   Calculator* getInstance() { return mpInst; };

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

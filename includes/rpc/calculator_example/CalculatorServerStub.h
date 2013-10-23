#ifndef CALCULATOR_SERVER_STUB_H
#define CALCULATOR_SERVER_STUB_H

#include "Calculator.h"
#include "RpcServerResource.h"
#include "CalculatorWrapper.h"

class CalculatorServerStub : public RpcServerResource
{
public:

   CalculatorServerStub() : RpcServerResource("calculator")
   {
   };
   
   virtual void registerActions()
   {
      addAction("add", CalculatorWrapper::add);
      addAction("sub", CalculatorWrapper::sub);
      addAction("mul", CalculatorWrapper::mul);
      addAction("div", CalculatorWrapper::div);
      addAction("getNumOps", CalculatorWrapper::getNumOps);
   };

protected:
   
   bool isValidInstance(int instId)
   {
      return (getInstance(instId) != NULL);
   }
   
   InstanceWrapper* createInstance(const Structure &params)
   {
      return new CalculatorWrapper(params);
   };
};

#endif // CALCULATOR_SERVER_STUB_H

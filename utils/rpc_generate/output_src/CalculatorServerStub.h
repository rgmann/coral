#ifndef CALCULATOR_SERVER_STUB_H
#define CALCULATOR_SERVER_STUB_H

#include "RpcServerResource.h"
#include "CalculatorWrapper.h"

class CalculatorServerStub : public RpcServerResource
{
public:

   CalculatorServerStub() : RpcServerResource("Calculator")
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
   
   InstanceWrapper* createInstance()
   {
      return new CalculatorWrapper();
   };
};

#endif // CALCULATOR_SERVER_STUB_H


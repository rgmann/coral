#ifndef CALCULATOR_SERVER_STUB_H
#define CALCULATOR_SERVER_STUB_H

#include "RpcServerResource.h"
#include "CalculatorWrapper.h"

using namespace liber::rpc;

struct CalculatorHookCb;
class CalculatorServerStub : public RpcServerResource {
public:

   CalculatorServerStub();
   ~CalculatorServerStub();
   
   virtual void registerActions();

   typedef void (*CalculatorHook)(CalculatorWrapper* pInst, void* pUserData);
   bool registerCtorHook(CalculatorHook pFunc, void* pUserData = NULL);
   bool registerDtorHook(CalculatorHook pFunc, void* pUserData = NULL);

protected:
  
   virtual bool tugCtorHook(InstanceWrapper* pInst);
   virtual bool tugDtorHook(InstanceWrapper* pInst);
 
   bool isValidInstance(int instId)
   {
      return (getInstance(instId) != NULL);
   }

   InstanceWrapper* createInstance();   

private:

   struct CalculatorHookCb* mpCreateHook;
   struct CalculatorHookCb* mpDestroyHook;

};

#endif // CALCULATOR_SERVER_STUB_H


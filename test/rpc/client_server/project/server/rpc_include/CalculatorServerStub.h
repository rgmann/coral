#ifndef CALCULATOR_SERVER_STUB_H
#define CALCULATOR_SERVER_STUB_H

#include "RpcServerResource.h"
#include "CalculatorWrapper.h"

struct CalculatorHookCb;
class CalculatorServerStub : public liber::rpc::RpcServerResource {
public:

   CalculatorServerStub();
   ~CalculatorServerStub();

   virtual void registerActions();

   typedef void (*CalculatorHook)(CalculatorWrapper*, void*);
   bool registerCtorHook(CalculatorHook pFunc, void* pUserData = NULL);
   bool registerDtorHook(CalculatorHook pFunc, void* pUserData = NULL);

protected:
   
   liber::rpc::InstanceWrapper* createInstance();

   bool tugCtorHook(liber::rpc::InstanceWrapper* pInst);
   bool tugDtorHook(liber::rpc::InstanceWrapper* pInst);

private:

   struct CalculatorHookCb* mpCreateHook;
   struct CalculatorHookCb* mpDestroyHook;
};

#endif // CALCULATOR_SERVER_STUB_H

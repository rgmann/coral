#include "CalculatorServerStub.h"

struct CalculatorHookCb {
   CalculatorServerStub::CalculatorHook pFunc;
   void* pUserData;
};

//-----------------------------------------------------------------------------
CalculatorServerStub::CalculatorServerStub()
: RpcServerResource("Calculator")
,  mpCreateHook(NULL)
,  mpDestroyHook(NULL)
{
   mpCreateHook = new CalculatorHookCb;
   mpCreateHook->pFunc = NULL;
   mpCreateHook->pUserData = NULL;

   mpDestroyHook = new CalculatorHookCb;
   mpDestroyHook->pFunc = NULL;
   mpDestroyHook->pUserData = NULL;
}

//-----------------------------------------------------------------------------
CalculatorServerStub::~CalculatorServerStub()
{
   if (mpCreateHook)
   {
      delete mpCreateHook;
      mpCreateHook = NULL;
   }

   if (mpDestroyHook)
   {
      delete mpDestroyHook;
      mpDestroyHook = NULL;
   }
}

//-----------------------------------------------------------------------------
void CalculatorServerStub::registerActions()
{
   addAction("add", CalculatorWrapper::addDelegate);
   addAction("sub", CalculatorWrapper::subDelegate);
   addAction("mul", CalculatorWrapper::mulDelegate);
   addAction("div", CalculatorWrapper::divDelegate);
   addAction("getNumOps", CalculatorWrapper::getNumOpsDelegate);
}

//-----------------------------------------------------------------------------
bool CalculatorServerStub::registerCtorHook(CalculatorHook pFunc, void* pUserData)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (mpCreateHook)
      {
         mpCreateHook->pFunc = pFunc;
         mpCreateHook->pUserData = pUserData;
      }

      mHookMutex.unlock();
   }

   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool CalculatorServerStub::registerDtorHook(CalculatorHook pFunc, void* pUserData)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (mpDestroyHook)
      {
         mpDestroyHook->pFunc = pFunc;
         mpDestroyHook->pUserData = pUserData;
      }

      mHookMutex.unlock();
   }

   return lbSuccess;
}

//-----------------------------------------------------------------------------
InstanceWrapper* CalculatorServerStub::createInstance()
{
   return new CalculatorWrapper();
}

//-----------------------------------------------------------------------------
bool CalculatorServerStub::tugCtorHook(InstanceWrapper* pInst)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (pInst && mpCreateHook && mpCreateHook->pFunc)
      {
         CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
         if (lpInst)
         {
            mpCreateHook->pFunc(lpInst, mpCreateHook->pUserData);
         }
      }

      mHookMutex.unlock();
   }

   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool CalculatorServerStub::tugDtorHook(InstanceWrapper* pInst)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);
   
   if (lbSuccess)
   {
      if (pInst && mpDestroyHook && mpDestroyHook->pFunc)
      {
         CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
         if (lpInst)
         {
            mpDestroyHook->pFunc(lpInst, mpDestroyHook->pUserData);
         }
      }

      mHookMutex.unlock();
   }

   return lbSuccess;
}


#include "RsyncServiceServerStub.h"

using namespace liber::rpc;

struct RsyncServiceHookCb {
   RsyncServiceServerStub::RsyncServiceHook pFunc;
   void* pUserData;
};

//-----------------------------------------------------------------------------
RsyncServiceServerStub::RsyncServiceServerStub()
: RpcServerResource("RsyncService")
,  mpCreateHook(NULL)
,  mpDestroyHook(NULL)
{
   mpCreateHook = new RsyncServiceHookCb;
   mpCreateHook->pFunc = NULL;
   mpCreateHook->pUserData = NULL;

   mpDestroyHook = new RsyncServiceHookCb;
   mpDestroyHook->pFunc = NULL;
   mpDestroyHook->pUserData = NULL;
}

//-----------------------------------------------------------------------------
RsyncServiceServerStub::~RsyncServiceServerStub()
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
void RsyncServiceServerStub::registerActions()
{
   addAction("queryFile", RsyncServiceWrapper::queryFileDelegate);
   addAction("pull", RsyncServiceWrapper::pullDelegate);
   addAction("push", RsyncServiceWrapper::pushDelegate);
}

//-----------------------------------------------------------------------------
bool RsyncServiceServerStub::
registerCtorHook(RsyncServiceHook pFunc, void* pUserData)
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
bool RsyncServiceServerStub::
registerDtorHook(RsyncServiceHook pFunc, void* pUserData)
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
InstanceWrapper* RsyncServiceServerStub::createInstance()
{
   return new RsyncServiceWrapper();
}

//-----------------------------------------------------------------------------
bool RsyncServiceServerStub::tugCtorHook(InstanceWrapper* pInst)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (pInst && mpCreateHook && mpCreateHook->pFunc)
      {
         RsyncServiceWrapper* lpInst = dynamic_cast<RsyncServiceWrapper*>(pInst);
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
bool RsyncServiceServerStub::tugDtorHook(InstanceWrapper* pInst)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (pInst && mpDestroyHook && mpDestroyHook->pFunc)
      {
         RsyncServiceWrapper* lpInst = dynamic_cast<RsyncServiceWrapper*>(pInst);
         if (lpInst)
         {
            mpDestroyHook->pFunc(lpInst, mpDestroyHook->pUserData);
         }
      }

      mHookMutex.unlock();
   }

   return lbSuccess;
}


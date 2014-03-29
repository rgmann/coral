#include "HeimdallControllerServerStub.h"

using namespace liber::rpc;

struct HeimdallControllerHookCb {
   HeimdallControllerServerStub::HeimdallControllerHook pFunc;
   void* pUserData;
};

//-----------------------------------------------------------------------------
HeimdallControllerServerStub::HeimdallControllerServerStub()
: RpcServerResource("HeimdallController")
,  mpCreateHook(NULL)
,  mpDestroyHook(NULL)
{
   mpCreateHook = new HeimdallControllerHookCb;
   mpCreateHook->pFunc = NULL;
   mpCreateHook->pUserData = NULL;

   mpDestroyHook = new HeimdallControllerHookCb;
   mpDestroyHook->pFunc = NULL;
   mpDestroyHook->pUserData = NULL;
}

//-----------------------------------------------------------------------------
HeimdallControllerServerStub::~HeimdallControllerServerStub()
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
void HeimdallControllerServerStub::registerActions()
{
   addAction("setLedState", HeimdallControllerWrapper::setLedStateDelegate);
   addAction("activateDoor", HeimdallControllerWrapper::activateDoorDelegate);
   addAction("enroll", HeimdallControllerWrapper::enrollDelegate);
   addAction("getUsers", HeimdallControllerWrapper::getUsersDelegate);
   addAction("removeOne", HeimdallControllerWrapper::removeOneDelegate);
   addAction("removeAll", HeimdallControllerWrapper::removeAllDelegate);
}

//-----------------------------------------------------------------------------
bool HeimdallControllerServerStub::
registerCtorHook(HeimdallControllerHook pFunc, void* pUserData)
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
bool HeimdallControllerServerStub::
registerDtorHook(HeimdallControllerHook pFunc, void* pUserData)
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
InstanceWrapper* HeimdallControllerServerStub::createInstance()
{
   return new HeimdallControllerWrapper();
}

//-----------------------------------------------------------------------------
bool HeimdallControllerServerStub::tugCtorHook(InstanceWrapper* pInst)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (pInst && mpCreateHook && mpCreateHook->pFunc)
      {
         HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
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
bool HeimdallControllerServerStub::tugDtorHook(InstanceWrapper* pInst)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (pInst && mpDestroyHook && mpDestroyHook->pFunc)
      {
         HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
         if (lpInst)
         {
            mpDestroyHook->pFunc(lpInst, mpDestroyHook->pUserData);
         }
      }

      mHookMutex.unlock();
   }

   return lbSuccess;
}


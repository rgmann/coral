#include "DatabaseServerStub.h"

using namespace liber::rpc;

struct DatabaseHookCb {
   DatabaseServerStub::DatabaseHook pFunc;
   void* pUserData;
};

//-----------------------------------------------------------------------------
DatabaseServerStub::DatabaseServerStub()
: RpcServerResource("Database")
,  mpCreateHook(NULL)
,  mpDestroyHook(NULL)
{
   mpCreateHook = new DatabaseHookCb;
   mpCreateHook->pFunc = NULL;
   mpCreateHook->pUserData = NULL;

   mpDestroyHook = new DatabaseHookCb;
   mpDestroyHook->pFunc = NULL;
   mpDestroyHook->pUserData = NULL;
}

//-----------------------------------------------------------------------------
DatabaseServerStub::~DatabaseServerStub()
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
void DatabaseServerStub::registerActions()
{
   addAction("create", DatabaseWrapper::createDelegate);
   addAction("read", DatabaseWrapper::readDelegate);
   addAction("update", DatabaseWrapper::updateDelegate);
   addAction("destroy", DatabaseWrapper::destroyDelegate);
}

//-----------------------------------------------------------------------------
bool DatabaseServerStub::
registerCtorHook(DatabaseHook pFunc, void* pUserData)
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
bool DatabaseServerStub::
registerDtorHook(DatabaseHook pFunc, void* pUserData)
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
InstanceWrapper* DatabaseServerStub::createInstance()
{
   return new DatabaseWrapper();
}

//-----------------------------------------------------------------------------
bool DatabaseServerStub::tugCtorHook(InstanceWrapper* pInst)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (pInst && mpCreateHook && mpCreateHook->pFunc)
      {
         DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
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
bool DatabaseServerStub::tugDtorHook(InstanceWrapper* pInst)
{
   bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

   if (lbSuccess)
   {
      if (pInst && mpDestroyHook && mpDestroyHook->pFunc)
      {
         DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
         if (lpInst)
         {
            mpDestroyHook->pFunc(lpInst, mpDestroyHook->pUserData);
         }
      }

      mHookMutex.unlock();
   }

   return lbSuccess;
}


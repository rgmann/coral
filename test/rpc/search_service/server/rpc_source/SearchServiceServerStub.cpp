#include "SearchServiceServerStub.h"

using namespace liber::rpc;
using namespace tutorial;

namespace tutorial {
struct SearchServiceHookCb {
  SearchServiceServerStub::SearchServiceHook pFunc;
  void* pUserData;
};
}

//-----------------------------------------------------------------------------
SearchServiceServerStub::SearchServiceServerStub()
: RpcServerResource("SearchService")
, mpCreateHook(NULL)
, mpDestroyHook(NULL)
{
  mpCreateHook = new SearchServiceHookCb;
  mpCreateHook->pFunc = NULL;
  mpCreateHook->pUserData = NULL;

  mpDestroyHook = new SearchServiceHookCb;
  mpDestroyHook->pFunc = NULL;
  mpDestroyHook->pUserData = NULL;
}

//-----------------------------------------------------------------------------
SearchServiceServerStub::
~SearchServiceServerStub()
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
void SearchServiceServerStub::registerActions()
{
  addAction("Search", SearchServiceWrapper::SearchDelegate);
}

//-----------------------------------------------------------------------------
bool SearchServiceServerStub::
registerCtorHook(SearchServiceHook pFunc, void* pUserData)
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
bool SearchServiceServerStub::
registerDtorHook(SearchServiceHook pFunc, void* pUserData)
{
  bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

  if (lbSuccess)
  {
    if (mpDestroyHook)
    {
      mpDestroyHook->pFunc     = pFunc;
      mpDestroyHook->pUserData = pUserData;
    }

    mHookMutex.unlock();
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
InstanceWrapper* SearchServiceServerStub::createInstance()
{
  return new SearchServiceWrapper();
}

//-----------------------------------------------------------------------------
bool SearchServiceServerStub::tugCtorHook(InstanceWrapper* pInst)
{
  bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

  if (lbSuccess)
  {
    if (pInst && mpCreateHook && mpCreateHook->pFunc)
    {
      SearchServiceWrapper* lpInst = dynamic_cast<SearchServiceWrapper*>(pInst);
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
bool SearchServiceServerStub::tugDtorHook(InstanceWrapper* pInst)
{
  bool lbSuccess = mHookMutex.lock(RpcServerResource::HookRegTimeoutMs);

  if (lbSuccess)
  {
    if (pInst && mpDestroyHook && mpDestroyHook->pFunc)
    {
      SearchServiceWrapper* lpInst = dynamic_cast<SearchServiceWrapper*>(pInst);
      if (lpInst)
      {
        mpDestroyHook->pFunc(lpInst, mpDestroyHook->pUserData);
      }
    }

    mHookMutex.unlock();
  }

  return lbSuccess;
}

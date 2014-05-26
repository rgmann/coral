#ifndef SEARCHSERVICE_SERVER_STUB_H
#define SEARCHSERVICE_SERVER_STUB_H

#include "RpcServerResource.h"
#include "SearchServiceWrapper.h"

namespace tutorial {

struct SearchServiceHookCb;

class SearchServiceServerStub
: public liber::rpc::RpcServerResource {
public:

  explicit SearchServiceServerStub();
  ~SearchServiceServerStub();

  virtual void registerActions();

  typedef void (*SearchServiceHook)(SearchServiceWrapper*, void*);
  bool registerCtorHook(SearchServiceHook pFunc, void* pUserData = NULL);
  bool registerDtorHook(SearchServiceHook pFunc, void* pUserData = NULL);

protected:

  liber::rpc::InstanceWrapper* createInstance();

  bool tugCtorHook(liber::rpc::InstanceWrapper* pInst);
  bool tugDtorHook(liber::rpc::InstanceWrapper* pInst);

private:

  struct SearchServiceHookCb* mpCreateHook;
  struct SearchServiceHookCb* mpDestroyHook;

}; // End SearchServiceServerStub

}  // End namespace tutorial


#endif // SEARCHSERVICE_SERVER_STUB_H

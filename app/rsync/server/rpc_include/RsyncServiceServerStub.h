#ifndef RSYNCSERVICE_SERVER_STUB_H
#define RSYNCSERVICE_SERVER_STUB_H

#include "RpcServerResource.h"
#include "RsyncServiceWrapper.h"

struct RsyncServiceHookCb;
class RsyncServiceServerStub : public liber::rpc::RpcServerResource {
public:

   RsyncServiceServerStub();
   ~RsyncServiceServerStub();

   virtual void registerActions();

   typedef void (*RsyncServiceHook)(RsyncServiceWrapper*, void*);
   bool registerCtorHook(RsyncServiceHook pFunc, void* pUserData = NULL);
   bool registerDtorHook(RsyncServiceHook pFunc, void* pUserData = NULL);

protected:
   
   liber::rpc::InstanceWrapper* createInstance();

   bool tugCtorHook(liber::rpc::InstanceWrapper* pInst);
   bool tugDtorHook(liber::rpc::InstanceWrapper* pInst);

private:

   struct RsyncServiceHookCb* mpCreateHook;
   struct RsyncServiceHookCb* mpDestroyHook;
};

#endif // RSYNCSERVICE_SERVER_STUB_H

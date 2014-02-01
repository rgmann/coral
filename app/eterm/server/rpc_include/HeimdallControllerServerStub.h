#ifndef HEIMDALLCONTROLLER_SERVER_STUB_H
#define HEIMDALLCONTROLLER_SERVER_STUB_H

#include "RpcServerResource.h"
#include "HeimdallControllerWrapper.h"

struct HeimdallControllerHookCb;
class HeimdallControllerServerStub : public liber::rpc::RpcServerResource {
public:

   HeimdallControllerServerStub();
   ~HeimdallControllerServerStub();

   virtual void registerActions();

   typedef void (*HeimdallControllerHook)(HeimdallControllerWrapper*, void*);
   bool registerCtorHook(HeimdallControllerHook pFunc, void* pUserData = NULL);
   bool registerDtorHook(HeimdallControllerHook pFunc, void* pUserData = NULL);

protected:
   
   liber::rpc::InstanceWrapper* createInstance();

   bool tugCtorHook(liber::rpc::InstanceWrapper* pInst);
   bool tugDtorHook(liber::rpc::InstanceWrapper* pInst);

private:

   struct HeimdallControllerHookCb* mpCreateHook;
   struct HeimdallControllerHookCb* mpDestroyHook;
};

#endif // HEIMDALLCONTROLLER_SERVER_STUB_H

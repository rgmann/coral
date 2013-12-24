#ifndef DATABASE_SERVER_STUB_H
#define DATABASE_SERVER_STUB_H

#include "RpcServerResource.h"
#include "DatabaseWrapper.h"

struct DatabaseHookCb;
class DatabaseServerStub : public liber::rpc::RpcServerResource {
public:

   DatabaseServerStub();
   ~DatabaseServerStub();

   virtual void registerActions();

   typedef void (*DatabaseHook)(DatabaseWrapper*, void*);
   bool registerCtorHook(DatabaseHook pFunc, void* pUserData = NULL);
   bool registerDtorHook(DatabaseHook pFunc, void* pUserData = NULL);

protected:
   
   liber::rpc::InstanceWrapper* createInstance();

   bool tugCtorHook(liber::rpc::InstanceWrapper* pInst);
   bool tugDtorHook(liber::rpc::InstanceWrapper* pInst);

private:

   struct DatabaseHookCb* mpCreateHook;
   struct DatabaseHookCb* mpDestroyHook;
};

#endif // DATABASE_SERVER_STUB_H

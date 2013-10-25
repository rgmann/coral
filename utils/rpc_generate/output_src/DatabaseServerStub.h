#ifndef DATABASE_SERVER_STUB_H
#define DATABASE_SERVER_STUB_H

#include "RpcServerResource.h"
#include "DatabaseWrapper.h"

class DatabaseServerStub : public RpcServerResource
{
public:

   DatabaseServerStub() : RpcServerResource("Database")
   {
   };
   
   virtual void registerActions()
   {
      addAction("create", DatabaseWrapper::create);
      addAction("read", DatabaseWrapper::read);
      addAction("update", DatabaseWrapper::update);
      addAction("destroy", DatabaseWrapper::destroy);
   };

protected:
   
   bool isValidInstance(int instId)
   {
      return (getInstance(instId) != NULL);
   }
   
   InstanceWrapper* createInstance()
   {
      return new DatabaseWrapper();
   };
};

#endif // DATABASE_SERVER_STUB_H


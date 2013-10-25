#ifndef DATABASE_CLIENT_STUB_H
#define DATABASE_CLIENT_STUB_H

#include "RpcClientResource.h"

class DatabaseClientStub : public RpcClientResource {
public:

   DatabaseClientStub(RpcClient &client);

   ~DatabaseClientStub();

   bool create(bool& result);
   bool read(bool& result);
   bool update(bool& result);
   bool destroy(bool& result);
};

#endif // DATABASE_CLIENT_STUB_H

#ifndef DATABASE_CLIENT_STUB_H
#define DATABASE_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "UserAttributes.h"

class DatabaseClientStub : public RpcClientResource {
public:

   DatabaseClientStub(RpcClient &client);

   ~DatabaseClientStub();

   bool create(std::string lastname, const UserAttributes& attrs, bool& result);
   bool read(std::string lastname, UserAttributes& attrs, bool& result);
   bool update(std::string lastname, const UserAttributes& attrs, bool& result);
   bool destroy(std::string lastname, bool& result);
};

#endif // DATABASE_CLIENT_STUB_H

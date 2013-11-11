#ifndef DATABASE_CLIENT_STUB_H
#define DATABASE_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "UserAttributes.h"

class DatabaseClientStub : public RpcClientResource {
public:

   DatabaseClientStub(RpcClient &client);

   ~DatabaseClientStub();

   bool create(std::string lastname, const UserAttributes& attrs) throw (RpcError);
   bool read(std::string lastname, UserAttributes& attrs) throw (RpcError);
   bool update(std::string lastname, const UserAttributes& attrs) throw (RpcError);
   bool destroy(std::string lastname) throw (RpcError);
};

#endif // DATABASE_CLIENT_STUB_H

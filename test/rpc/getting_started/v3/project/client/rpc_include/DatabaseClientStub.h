#ifndef DATABASE_CLIENT_STUB_H
#define DATABASE_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "rpc_getting_started.pb.h"

class DatabaseClientStub : public liber::rpc::RpcClientResource {
public:

   DatabaseClientStub(liber::rpc::RpcClient &client);

   ~DatabaseClientStub();

   void create(const rpc_getting_started::UserAttributes& request, rpc_getting_started::DbStatus& response) throw (liber::rpc::RpcException);
   void read(const rpc_getting_started::UserName& request, rpc_getting_started::UserAttributesList& response) throw (liber::rpc::RpcException);
   void update(const rpc_getting_started::UserAttributes& request, rpc_getting_started::DbStatus& response) throw (liber::rpc::RpcException);
   void destroy(const rpc_getting_started::UserName& request, rpc_getting_started::DbStatus& response) throw (liber::rpc::RpcException);
};

#endif // DATABASE_CLIENT_STUB_H

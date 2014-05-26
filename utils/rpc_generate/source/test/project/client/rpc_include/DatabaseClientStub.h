#ifndef DATABASE_CLIENT_STUB_H
#define DATABASE_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "rpc_calculator_app.pb.h"

class DatabaseClientStub : public liber::rpc::RpcClientResource {
public:

   DatabaseClientStub(liber::rpc::RpcClient &client);

   ~DatabaseClientStub();

   void create(const rpc_calculator_app::UserAttributes& request, rpc_calculator_app::DbStatus& response) throw (liber::rpc::RpcException);
   void read(const rpc_calculator_app::UserName& request, rpc_calculator_app::UserAttributesList& response) throw (liber::rpc::RpcException);
   void update(const rpc_calculator_app::UserAttributes& request, rpc_calculator_app::DbStatus& response) throw (liber::rpc::RpcException);
   void destroy(const rpc_calculator_app::UserName& request, rpc_calculator_app::DbStatus& response) throw (liber::rpc::RpcException);
};

#endif // DATABASE_CLIENT_STUB_H

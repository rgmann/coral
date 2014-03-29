#ifndef RSYNCSERVICE_CLIENT_STUB_H
#define RSYNCSERVICE_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "rpc_rsync_service.pb.h"

class RsyncServiceClientStub : public liber::rpc::RpcClientResource {
public:

   RsyncServiceClientStub(liber::rpc::RpcClient &client);

   ~RsyncServiceClientStub();

   void queryFile(const rpc_rsync_service::FileQuery& request, rpc_rsync_service::FileQueryResponse& response) throw (liber::rpc::RpcException);
   void pull(const rpc_rsync_service::JobStart& request, rpc_rsync_service::JobStartResponse& response) throw (liber::rpc::RpcException);
   void push(const rpc_rsync_service::JobStart& request, rpc_rsync_service::JobStartResponse& response) throw (liber::rpc::RpcException);
};

#endif // RSYNCSERVICE_CLIENT_STUB_H

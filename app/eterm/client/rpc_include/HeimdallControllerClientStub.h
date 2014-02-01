#ifndef HEIMDALLCONTROLLER_CLIENT_STUB_H
#define HEIMDALLCONTROLLER_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "rpc_eterm.pb.h"

class HeimdallControllerClientStub : public liber::rpc::RpcClientResource {
public:

   HeimdallControllerClientStub(liber::rpc::RpcClient &client);

   ~HeimdallControllerClientStub();

   void setLedState(const rpc_eterm::LedState& request, rpc_eterm::Status& response) throw (liber::rpc::RpcException);
   void getFingerprintStatus(const rpc_eterm::EmptyParams& request, rpc_eterm::Status& response) throw (liber::rpc::RpcException);
};

#endif // HEIMDALLCONTROLLER_CLIENT_STUB_H

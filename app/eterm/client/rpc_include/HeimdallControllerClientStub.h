#ifndef HEIMDALLCONTROLLER_CLIENT_STUB_H
#define HEIMDALLCONTROLLER_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "rpc_eterm.pb.h"

class HeimdallControllerClientStub : public liber::rpc::RpcClientResource {
public:

   HeimdallControllerClientStub(liber::rpc::RpcClient &client);

   ~HeimdallControllerClientStub();

   void setLedState(const rpc_eterm::HcSetLedState& request, rpc_eterm::Status& response) throw (liber::rpc::RpcException);
   void activateDoor(const rpc_eterm::EmptyParams& request, rpc_eterm::Status& response) throw (liber::rpc::RpcException);
   void enroll(const rpc_eterm::UserName& request, rpc_eterm::Status& response) throw (liber::rpc::RpcException);
   void getUsers(const rpc_eterm::EmptyParams& request, rpc_eterm::UserList& response) throw (liber::rpc::RpcException);
   void removeOne(const rpc_eterm::UserID& request, rpc_eterm::Status& response) throw (liber::rpc::RpcException);
   void removeAll(const rpc_eterm::EmptyParams& request, rpc_eterm::Status& response) throw (liber::rpc::RpcException);
};

#endif // HEIMDALLCONTROLLER_CLIENT_STUB_H

#ifndef HEIMDALLCONTROLLER_WRAPPER_H
#define HEIMDALLCONTROLLER_WRAPPER_H

#include "InstanceWrapper.h"
#include "rpc_eterm.pb.h"
#include "client.HeimdallController.h"

class HeimdallControllerWrapper : public liber::rpc::InstanceWrapper {
public:

   HeimdallControllerWrapper();
   ~HeimdallControllerWrapper();

   inline void setController(eterm::HeimdallController* pController) { mpController = pController; };

   bool initialize(const std::string& params);
   bool destroy(const std::string& params);

   static void setLedStateDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void activateDoorDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void enrollDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void getUsersDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void removeOneDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void removeAllDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);


private:

   void setLedState(const rpc_eterm::HcSetLedState& request, rpc_eterm::Status& response, liber::rpc::RpcException& e);
   void activateDoor(const rpc_eterm::EmptyParams& request, rpc_eterm::Status& response, liber::rpc::RpcException& e);
   void enroll(const rpc_eterm::UserName& request, rpc_eterm::Status& response, liber::rpc::RpcException& e);
   void getUsers(const rpc_eterm::EmptyParams& request, rpc_eterm::UserList& response, liber::rpc::RpcException& e);
   void removeOne(const rpc_eterm::UserID& request, rpc_eterm::Status& response, liber::rpc::RpcException& e);
   void removeAll(const rpc_eterm::EmptyParams& request, rpc_eterm::Status& response, liber::rpc::RpcException& e);


private:

   // TODO: Add additional instance variables here.
   eterm::HeimdallController* mpController;
};

#endif // HEIMDALLCONTROLLER_WRAPPER_H

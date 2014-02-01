#ifndef HEIMDALLCONTROLLER_WRAPPER_H
#define HEIMDALLCONTROLLER_WRAPPER_H

#include "InstanceWrapper.h"
#include "rpc_eterm.pb.h"
#include "HeimdallController.h"

class HeimdallControllerWrapper : public liber::rpc::InstanceWrapper {
public:

   HeimdallControllerWrapper();
   ~HeimdallControllerWrapper();

   bool initialize(const std::string& params);
   bool destroy(const std::string& params);

   static void setLedStateDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);


private:

   void setLedState(const rpc_eterm::LedState& request, rpc_eterm::Status& response, liber::rpc::RpcException& e);


private:

   // TODO: Add additional instance variables here.
   eterm::HeimdallController mController;

};

#endif // HEIMDALLCONTROLLER_WRAPPER_H

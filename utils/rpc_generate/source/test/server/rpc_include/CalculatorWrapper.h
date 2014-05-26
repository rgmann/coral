#ifndef CALCULATOR_WRAPPER_H
#define CALCULATOR_WRAPPER_H

#include "InstanceWrapper.h"
#include "rpc_calculator_app.pb.h"

class CalculatorWrapper : public liber::rpc::InstanceWrapper {
public:

   CalculatorWrapper();
   ~CalculatorWrapper();

   bool initialize(const std::string& params);
   bool destroy(const std::string& params);

   static void addDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);
   static void subDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);
   static void mulDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);
   static void divDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);
   static void getNumOpsDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, RpcException&);


private:

   liber::rpc::RpcException add(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response, liber::rpc::RpcException& e);
   liber::rpc::RpcException sub(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response, liber::rpc::RpcException& e);
   liber::rpc::RpcException mul(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response, liber::rpc::RpcException& e);
   liber::rpc::RpcException div(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response, liber::rpc::RpcException& e);
   liber::rpc::RpcException getNumOps(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response, liber::rpc::RpcException& e);


private:

   // TODO: Add additional instance variables here.
};

#endif // CALCULATOR_WRAPPER_H

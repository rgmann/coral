#ifndef CALCULATOR_WRAPPER_H
#define CALCULATOR_WRAPPER_H

#include "InstanceWrapper.h"
#include "rpc_getting_started.pb.h"
#include "Calculator.h"

class CalculatorWrapper : public liber::rpc::InstanceWrapper {
public:

   CalculatorWrapper();
   ~CalculatorWrapper();

   bool initialize(const std::string& params);
   bool destroy(const std::string& params);

   static void addDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void subDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void mulDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void divDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);
   static void getNumOpsDelegate(InstanceWrapper *pInst, const std::string& input, std::string& output, liber::rpc::RpcException&);


private:

   void add(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response, liber::rpc::RpcException& e);
   void sub(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response, liber::rpc::RpcException& e);
   void mul(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response, liber::rpc::RpcException& e);
   void div(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response, liber::rpc::RpcException& e);
   void getNumOps(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response, liber::rpc::RpcException& e);


private:

   // TODO: Add additional instance variables here.
   Calculator mCalculator;
};

#endif // CALCULATOR_WRAPPER_H

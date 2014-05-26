#ifndef CALCULATOR_CLIENT_STUB_H
#define CALCULATOR_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "rpc_calculator_app.pb.h"

class CalculatorClientStub : public liber::rpc::RpcClientResource {
public:

   CalculatorClientStub(liber::rpc::RpcClient &client);

   ~CalculatorClientStub();

   void add(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response) throw (liber::rpc::RpcException);
   void sub(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response) throw (liber::rpc::RpcException);
   void mul(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response) throw (liber::rpc::RpcException);
   void div(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response) throw (liber::rpc::RpcException);
   void getNumOps(const rpc_calculator_app::Parameters& request, rpc_calculator_app::Result& response) throw (liber::rpc::RpcException);
};

#endif // CALCULATOR_CLIENT_STUB_H

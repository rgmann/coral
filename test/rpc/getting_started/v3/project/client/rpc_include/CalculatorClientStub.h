#ifndef CALCULATOR_CLIENT_STUB_H
#define CALCULATOR_CLIENT_STUB_H

#include "RpcClientResource.h"
#include "rpc_getting_started.pb.h"

class CalculatorClientStub : public liber::rpc::RpcClientResource {
public:

   CalculatorClientStub(liber::rpc::RpcClient &client);

   ~CalculatorClientStub();

   void add(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response) throw (liber::rpc::RpcException);
   void sub(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response) throw (liber::rpc::RpcException);
   void mul(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response) throw (liber::rpc::RpcException);
   void div(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response) throw (liber::rpc::RpcException);
   void getNumOps(const rpc_getting_started::Parameters& request, rpc_getting_started::Result& response) throw (liber::rpc::RpcException);
};

#endif // CALCULATOR_CLIENT_STUB_H

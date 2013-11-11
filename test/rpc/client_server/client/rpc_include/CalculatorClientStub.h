#ifndef CALCULATOR_CLIENT_STUB_H
#define CALCULATOR_CLIENT_STUB_H

#include "RpcClientResource.h"

class CalculatorClientStub : public RpcClientResource {
public:

   CalculatorClientStub(RpcClient &client);

   ~CalculatorClientStub();

   i32 add(i32 a, i32 b) throw (RpcError);
   i32 sub(i32 a, i32 b) throw (RpcError);
   i32 mul(i32 a, i32 b) throw (RpcError);
   i32 div(i32 a, i32 b) throw (RpcError);
   i32 getNumOps() throw (RpcError);
};

#endif // CALCULATOR_CLIENT_STUB_H

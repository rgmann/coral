#ifndef CALCULATOR_CLIENT_STUB_H
#define CALCULATOR_CLIENT_STUB_H

#include "RpcClientResource.h"

class CalculatorClientStub : public RpcClientResource {
public:

   CalculatorClientStub(RpcClient &client);

   ~CalculatorClientStub();

   bool add(i32 a, i32 b, i32& result);
   bool sub(i32 a, i32 b, i32& result);
   bool mul(i32 a, i32 b, i32& result);
   bool div(i32 a, i32 b, i32& result);
   bool getNumOps(i32& result);
};

#endif // CALCULATOR_CLIENT_STUB_H

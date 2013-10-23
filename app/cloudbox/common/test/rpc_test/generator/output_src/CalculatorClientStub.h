#ifndef CALCULATOR_CLIENT_STUB_H
#define CALCULATOR_CLIENT_STUB_H

#include "RpcClientResource.h"

class CalculatorClientStub : public RpcClientResource {
public:

   Calculator(RpcClient &client);

   ~Calculator();

   void add(i32 a, i32 b, i32& result);
   void sub(i32 a, i32 b, i32& result);
   void mul(i32 a, i32 b, i32& result);
   void div(i32 a, i32 b, i32& result);
   void getNumOps(i32& result);
};

#endif // CALCULATOR_CLIENT_STUB_H

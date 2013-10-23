#ifndef CALCULATOR_CLIENT_STUB_H
#define CALCULATOR_CLIENT_STUB_H

#include "RpcClientResource.h"

class CalculatorClientStub : public RpcClientResource
{
public:

   CalculatorClientStub(RpcClient &client);
   
   ~CalculatorClientStub();

   bool add(int a, int b, int &result);

   bool sub(int a, int b, int &result);

   bool mul(int a, int b, int &result);

   bool div(int a, int b, int &result);
   
   bool getNumOps(int &numOps);

};

#endif // CALCULATOR_CLIENT_STUB_H

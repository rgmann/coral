#include "CalculatorClientStub.h"

//------------------------------------------------------------------------------
CalculatorClientStub::CalculatorClientStub(RpcClient &client)
: RpcClientResource(client, "calculator")
{
}

//------------------------------------------------------------------------------
CalculatorClientStub::~CalculatorClientStub()
{
}

//------------------------------------------------------------------------------
bool CalculatorClientStub::add(int a, int b, int &result)
{
   Structure lParams;
   RpcReturnValue lCallRetVal;

   lParams.set("a", a);
   lParams.set("b", b);

   return (call("add", lParams, lCallRetVal) && lCallRetVal.get(result));
}

//------------------------------------------------------------------------------
bool CalculatorClientStub::sub(int a, int b, int &result)
{
   Structure lParams;
   RpcReturnValue lCallRetVal;
   
   lParams.set("a", a);
   lParams.set("b", b);
   
   return (call("sub", lParams, lCallRetVal) && lCallRetVal.get(result));
}

//------------------------------------------------------------------------------
bool CalculatorClientStub::mul(int a, int b, int &result)
{  
   Structure lParams;
   RpcReturnValue lCallRetVal;
   
   lParams.set("a", a);
   lParams.set("b", b);
   
   return (call("mul", lParams, lCallRetVal) && lCallRetVal.get(result));
}

//------------------------------------------------------------------------------
bool CalculatorClientStub::div(int a, int b, int &result)
{  
   Structure lParams;
   RpcReturnValue lCallRetVal;
   
   lParams.set("a", a);
   lParams.set("b", b);
   
   return (call("div", lParams, lCallRetVal) && lCallRetVal.get(result));
}

//------------------------------------------------------------------------------
bool CalculatorClientStub::getNumOps(int &numOps)
{
   Structure lParams;
   RpcReturnValue lCallRetVal;
   
   return (call("getNumOps", lParams, lCallRetVal) && lCallRetVal.get(numOps));
}

#include "CalculatorClientStub.h"
#include "CalculatoraddParamListParamList.h"
#include "CalculatorsubParamListParamList.h"
#include "CalculatormulParamListParamList.h"
#include "CalculatordivParamListParamList.h"
#include "CalculatorgetNumOpsParamListParamList.h"

//------------------------------------------------------------------------------
CalculatorClientStub::CalculatorClientStub(RpcClient &client)
: RpcClientResource(client, "Calculator")
{
}

//------------------------------------------------------------------------------
CalculatorClientStub::~CalculatorClientStub()
{
}

//------------------------------------------------------------------------------
bool CalculatorClientStub::add(i32 a, i32 b, i32& result)
{
   CalculatoraddParamListParamList lParams(a, b);
   RpcReturnValue lCallRetVal;

   return (call("add", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}


//------------------------------------------------------------------------------
bool CalculatorClientStub::sub(i32 a, i32 b, i32& result)
{
   CalculatorsubParamListParamList lParams(a, b);
   RpcReturnValue lCallRetVal;

   return (call("sub", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}


//------------------------------------------------------------------------------
bool CalculatorClientStub::mul(i32 a, i32 b, i32& result)
{
   CalculatormulParamListParamList lParams(a, b);
   RpcReturnValue lCallRetVal;

   return (call("mul", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}


//------------------------------------------------------------------------------
bool CalculatorClientStub::div(i32 a, i32 b, i32& result)
{
   CalculatordivParamListParamList lParams(a, b);
   RpcReturnValue lCallRetVal;

   return (call("div", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}


//------------------------------------------------------------------------------
bool CalculatorClientStub::getNumOps(i32& result)
{
   CalculatorgetNumOpsParamListParamList lParams;
   RpcReturnValue lCallRetVal;

   return (call("getNumOps", lParams, lCallRetVal) &&
               lCallRetVal.get(result));
}




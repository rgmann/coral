#include "calculator.pb.h"
#include "CalculatorClientStub.h"

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
i32 CalculatorClientStub::add(i32 a, i32 b) throw (RpcError)
{
   rpctest::CalculatorAddIn lInParams;
   rpctest::CalculatorAddOut lOutParams;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.set_first(a);
   lInParams.set_second(b);

   if (call("add", lInParams, lOutParams))
   {
      lReturnValue = lOutParams.result();
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}

//------------------------------------------------------------------------------
i32 CalculatorClientStub::sub(i32 a, i32 b) throw (RpcError)
{
   rpctest::CalculatorSubIn lInParams;
   rpctest::CalculatorSubOut lOutParams;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.set_first(a);
   lInParams.set_second(b);

   if (call("sub", lInParams, lOutParams))
   {
      lReturnValue = lOutParams.result();
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}

//------------------------------------------------------------------------------
i32 CalculatorClientStub::mul(i32 a, i32 b) throw (RpcError)
{
   rpctest::CalculatorMulIn lInParams;
   rpctest::CalculatorMulOut lOutParams;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.set_first(a);
   lInParams.set_second(b);

   if (call("mul", lInParams, lOutParams))
   {
      lReturnValue = lOutParams.result();
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}

//------------------------------------------------------------------------------
i32 CalculatorClientStub::div(i32 a, i32 b) throw (RpcError)
{
   rpctest::CalculatorDivIn lInParams;
   rpctest::CalculatorDivOut lOutParams;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.set_first(a);
   lInParams.set_second(b);

   if (call("div", lInParams, lOutParams))
   {
      lReturnValue = lOutParams.result();
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}

//------------------------------------------------------------------------------
i32 CalculatorClientStub::getNumOps() throw (RpcError)
{
   rpctest::CalculatorGetNumOpsIn lInParams;
   rpctest::CalculatorGetNumOpsOut lOutParams;
   i32 lReturnValue;

   // Marshall the parameters.

   if (call("getNumOps", lInParams, lOutParams))
   {
      lReturnValue = lOutParams.result();
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}



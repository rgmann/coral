#include "CalculatorClientStub.h"
#include "CalculatorAddInput.h"
#include "CalculatorAddOutput.h"
#include "CalculatorSubInput.h"
#include "CalculatorSubOutput.h"
#include "CalculatorMulInput.h"
#include "CalculatorMulOutput.h"
#include "CalculatorDivInput.h"
#include "CalculatorDivOutput.h"
#include "CalculatorGetnumopsInput.h"
#include "CalculatorGetnumopsOutput.h"

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
   CalculatorAddInput lInParams;
   Structure lOutStructure;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.marshall(a, b);

   if (call("add", lInParams.getParameters(), lOutStructure))
   {
      CalculatorAddOutput lOutParams(lOutStructure);
      lOutParams.getReturnValue(lReturnValue);
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
   CalculatorSubInput lInParams;
   Structure lOutStructure;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.marshall(a, b);

   if (call("sub", lInParams.getParameters(), lOutStructure))
   {
      CalculatorSubOutput lOutParams(lOutStructure);
      lOutParams.getReturnValue(lReturnValue);
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
   CalculatorMulInput lInParams;
   Structure lOutStructure;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.marshall(a, b);

   if (call("mul", lInParams.getParameters(), lOutStructure))
   {
      CalculatorMulOutput lOutParams(lOutStructure);
      lOutParams.getReturnValue(lReturnValue);
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
   CalculatorDivInput lInParams;
   Structure lOutStructure;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.marshall(a, b);

   if (call("div", lInParams.getParameters(), lOutStructure))
   {
      CalculatorDivOutput lOutParams(lOutStructure);
      lOutParams.getReturnValue(lReturnValue);
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
   CalculatorGetnumopsInput lInParams;
   Structure lOutStructure;
   i32 lReturnValue;

   // Marshall the parameters.
   lInParams.marshall();

   if (call("getNumOps", lInParams.getParameters(), lOutStructure))
   {
      CalculatorGetnumopsOutput lOutParams(lOutStructure);
      lOutParams.getReturnValue(lReturnValue);
   }
   else
   {
      throw getLastError();
   }

   return lReturnValue;
}




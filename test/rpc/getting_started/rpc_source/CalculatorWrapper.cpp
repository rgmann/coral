#include "CalculatorWrapper.h"
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

//-----------------------------------------------------------------------------
CalculatorWrapper::CalculatorWrapper()
:  InstanceWrapper(),
   mpInst(NULL)
{
}

//-----------------------------------------------------------------------------
CalculatorWrapper::~CalculatorWrapper()
{
}

//-----------------------------------------------------------------------------
bool CalculatorWrapper::initialize(const Structure &params)
{
   mpInst = new Calculator(/* TODO: Pass correct parameters*/);
   return (mpInst != NULL);
}

//-----------------------------------------------------------------------------
bool CalculatorWrapper::destroy(const Structure &params)
{
   if (mpInst)
   {
      delete mpInst;
      mpInst = NULL;
   }
   return true;
}

//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
addDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   i32 lReturnValue /* = TODO: If applicable, set default */;
   i32 a = 0;
   i32 b = 0;

   CalculatorAddInput inParams(input);
   if (!inParams.unmarshall(a, b))
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->add(a, b);

   CalculatorAddOutput outParams;
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}


//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
subDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   i32 lReturnValue /* = TODO: If applicable, set default */;
   i32 a = 0;
   i32 b = 0;

   CalculatorSubInput inParams(input);
   if (!inParams.unmarshall(a, b))
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->sub(a, b);

   CalculatorSubOutput outParams;
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}


//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
mulDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   i32 lReturnValue /* = TODO: If applicable, set default */;
   i32 a = 0;
   i32 b = 0;

   CalculatorMulInput inParams(input);
   if (!inParams.unmarshall(a, b))
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->mul(a, b);

   CalculatorMulOutput outParams;
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}


//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
divDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   i32 lReturnValue /* = TODO: If applicable, set default */;
   i32 a = 0;
   i32 b = 0;

   CalculatorDivInput inParams(input);
   if (!inParams.unmarshall(a, b))
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->div(a, b);

   CalculatorDivOutput outParams;
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}


//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
getNumOpsDelegate
(InstanceWrapper* pInst, const Structure& input, Structure& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   i32 lReturnValue /* = TODO: If applicable, set default */;

   CalculatorGetnumopsInput inParams(input);
   if (!inParams.unmarshall())
   {
      return MissingParameters;
   }

   lReturnValue = lpInst->getNumOps();

   CalculatorGetnumopsOutput outParams;
   outParams.setReturnValue(lReturnValue);
   output = outParams.getParameters();

   return NoException;
}




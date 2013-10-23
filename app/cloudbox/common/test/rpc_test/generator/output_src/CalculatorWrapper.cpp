#include "CalculatorWrapper.h"

//-----------------------------------------------------------------------------
CalculatorWrapper::CalculatorWrapper()
:  InstanceWrapper(),
   mpInst(NULL)
{
};

//-----------------------------------------------------------------------------
CalculatorWrapper::~CalculatorWrapper()
{
   if (mpInst)
   {
      delete mpInst;
      mpInst = NULL;
   }
}

//-----------------------------------------------------------------------------
bool CalculatorWrapper::initialize(const Structure &params)
{
   mpInst = new Calculator(/* TODO: Pass correct parameters*/);
};

//-----------------------------------------------------------------------------
void CalculatorWrapper::add(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   i32 lRetval /* = TODO: If applicable, set default */;
   i32 a;
   i32 b;

   reinterpret_cast<CalculatorAddParamList&>(params).get(a, b);

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void CalculatorWrapper::sub(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   i32 lRetval /* = TODO: If applicable, set default */;
   i32 a;
   i32 b;

   reinterpret_cast<CalculatorSubParamList&>(params).get(a, b);

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void CalculatorWrapper::mul(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   i32 lRetval /* = TODO: If applicable, set default */;
   i32 a;
   i32 b;

   reinterpret_cast<CalculatorMulParamList&>(params).get(a, b);

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void CalculatorWrapper::div(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   i32 lRetval /* = TODO: If applicable, set default */;
   i32 a;
   i32 b;

   reinterpret_cast<CalculatorDivParamList&>(params).get(a, b);

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void CalculatorWrapper::getNumOps(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   i32 lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<CalculatorGetnumopsParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}



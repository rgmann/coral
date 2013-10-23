#include "CalculatorWrapper.h"
#include "CalculatorAddParamList.h"
#include "CalculatorSubParamList.h"
#include "CalculatorMulParamList.h"
#include "CalculatorDivParamList.h"
#include "CalculatorGetnumopsParamList.h"

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
void CalculatorWrapper::add(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   i32 lRetval /* = TODO: If applicable, set default */;
   i32 a;
   i32 b;

   reinterpret_cast<CalculatorAddParamList&>(params).get(a, b);

   /* TODO: Perform action. */
   lRetval = reinterpret_cast<CalculatorWrapper*>(pInst)->mpInst->add(a, b);

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
   lRetval = reinterpret_cast<CalculatorWrapper*>(pInst)->mpInst->sub(a, b);

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
   lRetval = reinterpret_cast<CalculatorWrapper*>(pInst)->mpInst->mul(a, b);

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
   lRetval = reinterpret_cast<CalculatorWrapper*>(pInst)->mpInst->div(a, b);

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
   lRetval = reinterpret_cast<CalculatorWrapper*>(pInst)->mpInst->getNumOps();

   retobj.set(lRetval);
}



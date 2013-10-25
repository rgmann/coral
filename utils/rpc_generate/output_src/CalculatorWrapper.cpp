#include "CalculatorWrapper.h"
#include "CalculatoraddParamListParamList.h"
#include "CalculatorsubParamListParamList.h"
#include "CalculatormulParamListParamList.h"
#include "CalculatordivParamListParamList.h"
#include "CalculatorgetNumOpsParamListParamList.h"

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

   reinterpret_cast<CalculatoraddParamList&>(params).get(a, b);

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

   reinterpret_cast<CalculatorsubParamList&>(params).get(a, b);

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

   reinterpret_cast<CalculatormulParamList&>(params).get(a, b);

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

   reinterpret_cast<CalculatordivParamList&>(params).get(a, b);

   /* TODO: Perform action. */

   retobj.set(lRetval);
}

//-----------------------------------------------------------------------------
void CalculatorWrapper::getNumOps(InstanceWrapper *pInst,
                                               Structure       &params, 
                                               RpcReturnValue  &retobj)
{
   i32 lRetval /* = TODO: If applicable, set default */;

   reinterpret_cast<CalculatorgetNumOpsParamList&>(params).get();

   /* TODO: Perform action. */

   retobj.set(lRetval);
}



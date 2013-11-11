#include "CalculatorWrapper.h"

//-----------------------------------------------------------------------------
i32 CalculatorWrapper::
add(i32 a, i32 b)
{
   i32 lReturnValue  = mpInst->add(a, b);

   return lReturnValue;
}


//-----------------------------------------------------------------------------
i32 CalculatorWrapper::
sub(i32 a, i32 b)
{
   i32 lReturnValue = mpInst->sub(a, b);

   return lReturnValue;
}


//-----------------------------------------------------------------------------
i32 CalculatorWrapper::
mul(i32 a, i32 b)
{
   i32 lReturnValue = mpInst->mul(a, b);

   return lReturnValue;
}


//-----------------------------------------------------------------------------
i32 CalculatorWrapper::
div(i32 a, i32 b)
{
   i32 lReturnValue = mpInst->div(a, b);

   return lReturnValue;
}


//-----------------------------------------------------------------------------
i32 CalculatorWrapper::
getNumOps()
{
   i32 lReturnValue = mpInst->getNumOps();

   return lReturnValue;
}




#include "Calculator.h"

//------------------------------------------------------------------------------
Calculator::Calculator()
: mnNumOps(0)
{
}

//------------------------------------------------------------------------------
int Calculator::add(int a, int b)
{
   mnNumOps++;
   return a + b;
}

//------------------------------------------------------------------------------
int Calculator::sub(int a, int b)
{
   mnNumOps++;
   return a - b;
}

//------------------------------------------------------------------------------
int Calculator::mul(int a, int b)
{
   mnNumOps++;
   return a * b;
}

//------------------------------------------------------------------------------
int Calculator::div(int a, int b)
{
   mnNumOps++;
   if (b == 0) return 0;
   return a / b;
}

//------------------------------------------------------------------------------
int Calculator::getNumOps() const
{
   return mnNumOps;
}


#include <iostream>
#include "Calculator.h"

//------------------------------------------------------------------------------
Calculator::Calculator()
: mnNumOps(0)
{
}

//------------------------------------------------------------------------------
int Calculator::add(const std::vector<int>& values)
{
   int sum = 0;
   std::vector<int>::const_iterator lIt = values.begin();
   for (; lIt != values.end(); lIt++)
   {
      mnNumOps++;
      sum += *lIt;
   }
   return sum;
}

//------------------------------------------------------------------------------
int Calculator::sub(const std::vector<int>& values)
{
   int sum = 0;
   std::vector<int>::const_iterator lIt = values.begin();
   for (; lIt != values.end(); lIt++)
   {
      if (lIt == values.begin()) sum = *lIt;
      else {
         mnNumOps++;
         sum -= *lIt;
      }
   }
   return sum;
}

//------------------------------------------------------------------------------
int Calculator::mul(const std::vector<int>& values)
{
   int prod = 1;
   std::vector<int>::const_iterator lIt = values.begin();
   for (; lIt != values.end(); lIt++)
   {
      mnNumOps++;
      prod *= *lIt;
   }
   return prod;
}

//------------------------------------------------------------------------------
int Calculator::div(const std::vector<int>& values)
{
   int quot = 0;
   std::vector<int>::const_iterator lIt = values.begin();
   for (; lIt != values.end(); lIt++)
   {
      if (lIt == values.begin()) quot = *lIt;
      else if (*lIt == 0) continue;
      else {
         mnNumOps++;
         quot /= *lIt;
      }
   }
   return quot;
}

//------------------------------------------------------------------------------
int Calculator::getNumOps() const
{
   return mnNumOps;
}


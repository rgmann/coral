#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <vector>

class Calculator
{
public:

   Calculator();

   int add(const std::vector<int>& values);

   int sub(const std::vector<int>& values);

   int mul(const std::vector<int>& values);

   int div(const std::vector<int>& values);

   int getNumOps() const;

private:
   
   int mnNumOps;
};

#endif // CALCULATOR_H

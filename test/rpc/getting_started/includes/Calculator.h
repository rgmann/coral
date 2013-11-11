#ifndef CALCULATOR_H
#define CALCULATOR_H

class Calculator
{
public:

   Calculator();

   int add(int a, int b);

   int sub(int a, int b);

   int mul(int a, int b);

   int div(int a, int b);

   int getNumOps() const;

private:
   
   int mnNumOps;
};

#endif // CALCULATOR_H

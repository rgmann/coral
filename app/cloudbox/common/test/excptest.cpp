#include <string>
#include <iostream>

enum RpcException {
   Timeout,
   NoMethod,
   NoClass
};

inline std::string toString(RpcException exception)
{
   std::string exceptionStr = "";
   switch (exception)
   {
   case Timeout: exceptionStr = "Timeout"; break;
   case NoMethod: exceptionStr = "NoMethod"; break;
   case NoClass: exceptionStr = "NoClass"; break;
   default: exceptionStr = "Unknown"; break;
   }

   return exceptionStr;
}

int doop(std::string op, int a, int b) throw (RpcException)
{
   if (op == "add") return a + b;
   else if (op == "sub") return a - b;
   else throw NoMethod;
}

int rpcadd(int a, int b) throw (RpcException)
{
   try {
      return doop("add", a, b);
   } catch (RpcException exception) {
      throw exception;
   }
}

int rpcdiv(int a, int b) throw (RpcException)
{
   try {
      return doop("div", a, b);
   } catch (RpcException exception) {
       throw exception;
   }
}

int main()
{
   try {
      std::cout << "9 + 8 = " << rpcadd(9, 8) << std::endl;
   } catch (RpcException exception) {
      std::cout << "RpcException: " << toString(exception) << " at " << __LINE__ << std::endl;
   }

   try {
      std::cout << "10 / 5 = " << rpcdiv(10, 5) << std::endl;
   } catch (RpcException exception) {
      std::cout << "RpcException: " << toString(exception) << " at " << __LINE__ << std::endl;
   }

   return 0;
}


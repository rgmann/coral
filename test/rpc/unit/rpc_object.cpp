#include <iostream>
#include "RpcObject.h"         

#define ASSERT_EQUAL(t, a, b) check_equal<t>(a, b, __LINE__)

int gPassCount = 0;
int gFailCount = 0;
int gTotalCount = 0;

template <class T>
void check_equal(T a, T b, int line)
{
   if (a == b) gPassCount++;
   else
   {
      std::cout << "FAIL(" << line << "): expected = " << a 
                << ", actual = " << b << std::endl;
      gFailCount++;
   }
   gTotalCount++;
}

void print_summary()
{
   std::cout << "--------------------------------------------------------" << std::endl;
   std::cout << "TOTL: " << gTotalCount << std::endl;
   std::cout << "PASS: " << gPassCount << std::endl;
   std::cout << "FAIL: " << gFailCount << std::endl;
   std::cout << "--------------------------------------------------------" << std::endl;
}

int main()
{
   liber::rpc::RpcObject srcObject;
   liber::rpc::RpcObject dstObject;

   srcObject.resource = "calculator";
   srcObject.action = "add";
   srcObject.instanceId = 2;
   srcObject.rpcId = 21754;
   srcObject.error.exceptionId = NoException;

   std::string data;
   data = srcObject.serialize();
   std::cout << "data.size() = " << data.size() << std::endl;
   ASSERT_EQUAL(bool, true, dstObject.deserialize(data));

   ASSERT_EQUAL(std::string, "calculator", dstObject.resource);
   ASSERT_EQUAL(std::string, "add", dstObject.action);
   ASSERT_EQUAL(i32, 2, dstObject.instanceId);
   ASSERT_EQUAL(i32, 21754, dstObject.rpcId);
   ASSERT_EQUAL(RpcException, NoException, dstObject.error.exceptionId);

   print_summary();

   return 0;
}



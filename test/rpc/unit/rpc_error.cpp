#include <iostream>
#include "RpcError.h"         

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
   coral::rpc::RpcError srcError;
   coral::rpc::RpcError dstError;

   srcError.reporter = coral::rpc::RpcError::Server;
   srcError.exceptionId = InvalidClassname;
   srcError.message = "Invalid class name.";

   std::string data;
   data = srcError.serialize();
   ASSERT_EQUAL(bool, true, dstError.deserialize(data));

   std::cout << "data.size() = " << data.size() << std::endl;
   ASSERT_EQUAL(i32, coral::rpc::RpcError::Server, dstError.reporter);
   ASSERT_EQUAL(i32, InvalidClassname, dstError.exceptionId);
   ASSERT_EQUAL(std::string, "Invalid class name.", dstError.message);

   print_summary();

   return 0;
}



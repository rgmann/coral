// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 

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
   coral::rpc::RpcObject srcObject;
   coral::rpc::RpcObject dstObject;

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



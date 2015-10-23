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

using namespace coral::rpc;

void traceframetest_run()
{
   std::cout << "traceframetest_run:" << std::endl;
   TraceFrame frame1("TestClass", "TestMethod", __FILE__, 135);
   std::string frame1data = frame1.serialize();

   TraceFrame frame2;
   if (!frame2.deserialize(frame1data))
     std::cout << "Failed to deserialize frame data." << std::endl;
   else
   {
      std::cout << "frame2.mClassName = " << frame2.mClassName << std::endl;
      std::cout << "frame2.mMethodName = " << frame2.mMethodName << std::endl;
      std::cout << "frame2.mFileName = " << frame2.mFileName << std::endl;
      std::cout << "frame2.mLineNumber = " << frame2.mLineNumber << std::endl;
   }
   std::cout << std::endl;
}

void rpcexceptiontest_run()
{
   std::cout << "rpcexceptiontest_run:" << std::endl;
   RpcException exc1;
   exc1.id = UnknownMethod;
   exc1.message = "test error message";
   exc1.reporter = RpcException::Server;
   exc1.pushFrame(TraceFrame("TestClass", "TestMethod", __FILE__, 135));
   std::string exc1data = exc1.serialize();

   RpcException exc2;
   if (!exc2.deserialize(exc1data))
     std::cout << "Failed to deserialize exception data." << std::endl;
   else
   {
      std::cout << "exc2.id = " << ToRpcErrorString(exc2.id) << std::endl;
      std::cout << "exc2.message = " << exc2.message << std::endl;
      std::cout << "exc2.reporter = " << exc2.reporter << std::endl;
      std::vector<std::string> strTrace = exc2.traceString();
      std::vector<std::string>::iterator lIt = strTrace.begin();
      int nDepth = 0;
      for (; lIt != strTrace.end(); lIt++)
      {
         std::cout << "frame" << nDepth << ":  " << *lIt << std::endl;
         nDepth++;
      }
   }
   std::cout << std::endl;
}

void rpcobjecttest_run()
{
   std::cout << "rpcobjecttest_run:" << std::endl;
   RpcObject obj1;
   obj1.callInfo().resource = "TestResource";
   obj1.callInfo().action = "TestAction";
   obj1.callInfo().uuid = 5595;
   obj1.callInfo().rpcId = 1234;
   obj1.exception().id = UnknownMethod;
   obj1.exception().message = "test error message";
   obj1.exception().reporter = RpcException::Server;
   obj1.exception().pushFrame(TraceFrame("TestClass", "TestMethod", __FILE__, 135));
   obj1.mMessage = "1dkghu385#fksie";
   std::string obj1data = obj1.serialize();


   RpcObject obj2;
   if (!obj2.deserialize(obj1data))
      std::cout << "Failed to deserialize object data." << std::endl;
   else
   {
      std::cout << "obj2.callInfo().resource = " << obj2.callInfo().resource << std::endl;
      std::cout << "obj2.callInfo().action = " << obj2.callInfo().action << std::endl;
      std::cout << "obj2.callInfo().uuid = " << obj2.callInfo().uuid << std::endl;
      std::cout << "obj2.callInfo().rpcId = " << obj2.callInfo().rpcId << std::endl;
      std::cout << "obj2.mMessage = " << obj2.mMessage << std::endl;
      std::cout << "obj2.exception().id = " << ToRpcErrorString(obj2.exception().id) << std::endl;
      std::cout << "obj2.exception().message = " << obj2.exception().message << std::endl;
      std::cout << "obj2.exception().reporter = " << obj2.exception().reporter << std::endl;
      std::vector<std::string> strTrace = obj2.exception().traceString();
      std::vector<std::string>::iterator lIt = strTrace.begin();
      int nDepth = 0;
      for (; lIt != strTrace.end(); lIt++)
      {
         std::cout << "obj2.exception(): frame" << nDepth << ":  " << *lIt << std::endl;
         nDepth++;
      }
   }
   std::cout << std::endl;
}

int main()
{
   traceframetest_run();
   rpcexceptiontest_run();
   rpcobjecttest_run();

   return 0;
}


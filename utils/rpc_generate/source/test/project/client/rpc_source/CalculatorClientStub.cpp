#include "rpc_calculator_app.pb.h"
#include "CalculatorClientStub.h"

using namespace liber::rpc;
using namespace rpc_calculator_app;

//------------------------------------------------------------------------------
CalculatorClientStub::CalculatorClientStub(RpcClient &client)
: RpcClientResource(client, "Calculator")
{
}

//------------------------------------------------------------------------------
CalculatorClientStub::~CalculatorClientStub()
{
}

//------------------------------------------------------------------------------
void CalculatorClientStub::add(const Parameters& request, Result& response) throw (RpcException)
{
   if (!call("add", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void CalculatorClientStub::sub(const Parameters& request, Result& response) throw (RpcException)
{
   if (!call("sub", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void CalculatorClientStub::mul(const Parameters& request, Result& response) throw (RpcException)
{
   if (!call("mul", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void CalculatorClientStub::div(const Parameters& request, Result& response) throw (RpcException)
{
   if (!call("div", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void CalculatorClientStub::getNumOps(const Parameters& request, Result& response) throw (RpcException)
{
   if (!call("getNumOps", request, response))
   {
      throw getLastError();
   }
}




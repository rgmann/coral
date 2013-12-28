#include "rpc_getting_started.pb.h"
#include "CalculatorWrapper.h"

using namespace liber::rpc;
using namespace rpc_getting_started;

//-----------------------------------------------------------------------------
CalculatorWrapper::CalculatorWrapper()
:  InstanceWrapper()
{
}

//-----------------------------------------------------------------------------
CalculatorWrapper::~CalculatorWrapper()
{
}

//-----------------------------------------------------------------------------
bool CalculatorWrapper::initialize(const std::string& params)
{
   bool lbSuccess = true;

   /* TODO: Perform instance initialization. */

   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool CalculatorWrapper::destroy(const std::string& params)
{
   bool lbSuccess = true;

   /* TODO: Destroy instance. */

   return lbSuccess;
}

//-----------------------------------------------------------------------------
void CalculatorWrapper::
addDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   e.pushFrame(TraceFrame("CalculatorWrapper", "add",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->add(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
subDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   e.pushFrame(TraceFrame("CalculatorWrapper", "sub",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->sub(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
mulDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   e.pushFrame(TraceFrame("CalculatorWrapper", "mul",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->mul(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
divDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   e.pushFrame(TraceFrame("CalculatorWrapper", "div",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->div(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
getNumOpsDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   e.pushFrame(TraceFrame("CalculatorWrapper", "getNumOps",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->getNumOps(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}




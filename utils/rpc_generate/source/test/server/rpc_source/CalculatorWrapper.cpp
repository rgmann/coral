#include "rpc_calculator_app.pb.h"
#include "CalculatorWrapper.h"

using namespace liber::rpc;
using namespace rpc_calculator_app;

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

   return true;
}

//-----------------------------------------------------------------------------
void CalculatorWrapper::
addDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->add(lRequest, lResponse);
   lResponse.SerializeToString(&response);
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
subDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->sub(lRequest, lResponse);
   lResponse.SerializeToString(&response);
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
mulDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->mul(lRequest, lResponse);
   lResponse.SerializeToString(&response);
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
divDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->div(lRequest, lResponse);
   lResponse.SerializeToString(&response);
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
getNumOpsDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   Parameters lRequest;
   Result lResponse;

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->getNumOps(lRequest, lResponse);
   lResponse.SerializeToString(&response);
}




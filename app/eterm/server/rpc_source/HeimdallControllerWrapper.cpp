#include "rpc_eterm.pb.h"
#include "HeimdallControllerWrapper.h"

using namespace liber::rpc;
using namespace rpc_eterm;

//-----------------------------------------------------------------------------
HeimdallControllerWrapper::HeimdallControllerWrapper()
:  InstanceWrapper()
{
}

//-----------------------------------------------------------------------------
HeimdallControllerWrapper::~HeimdallControllerWrapper()
{
}

//-----------------------------------------------------------------------------
bool HeimdallControllerWrapper::initialize(const std::string& params)
{
   bool lbSuccess = true;

   /* TODO: Perform instance initialization. */

   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool HeimdallControllerWrapper::destroy(const std::string& params)
{
   bool lbSuccess = true;

   /* TODO: Destroy instance. */

   return lbSuccess;
}

//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
setLedStateDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
   LedState lRequest;
   Status lResponse;

   e.pushFrame(TraceFrame("HeimdallControllerWrapper", "setLedState",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->setLedState(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
getFingerprintStatusDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
   EmptyParams lRequest;
   Status lResponse;

   e.pushFrame(TraceFrame("HeimdallControllerWrapper", "getFingerprintStatus",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->getFingerprintStatus(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}




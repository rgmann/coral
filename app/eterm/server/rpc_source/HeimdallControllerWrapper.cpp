#include "rpc_eterm.pb.h"
#include "HeimdallControllerWrapper.h"

using namespace liber::rpc;
using namespace rpc_eterm;
using namespace eterm;

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
   HcSetLedState lRequest;
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
activateDoorDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
   EmptyParams lRequest;
   Status lResponse;

   e.pushFrame(TraceFrame("HeimdallControllerWrapper", "activateDoor",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->activateDoor(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
enrollDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
   UserName lRequest;
   Status lResponse;

   e.pushFrame(TraceFrame("HeimdallControllerWrapper", "enroll",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->enroll(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
getUsersDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
   EmptyParams lRequest;
   UserList lResponse;

   e.pushFrame(TraceFrame("HeimdallControllerWrapper", "getUsers",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->getUsers(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
removeOneDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
   UserID lRequest;
   Status lResponse;

   e.pushFrame(TraceFrame("HeimdallControllerWrapper", "removeOne",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->removeOne(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void HeimdallControllerWrapper::
removeAllDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   HeimdallControllerWrapper* lpInst = dynamic_cast<HeimdallControllerWrapper*>(pInst);
   EmptyParams lRequest;
   Status lResponse;

   e.pushFrame(TraceFrame("HeimdallControllerWrapper", "removeAll",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->removeAll(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}




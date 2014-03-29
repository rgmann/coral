#include "rpc_rsync_service.pb.h"
#include "RsyncServiceWrapper.h"

using namespace liber::rpc;
using namespace rpc_rsync_service;

//-----------------------------------------------------------------------------
RsyncServiceWrapper::RsyncServiceWrapper()
:  InstanceWrapper()
{
}

//-----------------------------------------------------------------------------
RsyncServiceWrapper::~RsyncServiceWrapper()
{
}

//-----------------------------------------------------------------------------
bool RsyncServiceWrapper::initialize(const std::string& params)
{
   bool lbSuccess = true;

   /* TODO: Perform instance initialization. */

   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RsyncServiceWrapper::destroy(const std::string& params)
{
   bool lbSuccess = true;

   /* TODO: Destroy instance. */

   return lbSuccess;
}

//-----------------------------------------------------------------------------
void RsyncServiceWrapper::
queryFileDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   RsyncServiceWrapper* lpInst = dynamic_cast<RsyncServiceWrapper*>(pInst);
   FileQuery lRequest;
   FileQueryResponse lResponse;

   e.pushFrame(TraceFrame("RsyncServiceWrapper", "queryFile",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->queryFile(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void RsyncServiceWrapper::
pullDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   RsyncServiceWrapper* lpInst = dynamic_cast<RsyncServiceWrapper*>(pInst);
   JobStart lRequest;
   JobStartResponse lResponse;

   e.pushFrame(TraceFrame("RsyncServiceWrapper", "pull",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->pull(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void RsyncServiceWrapper::
pushDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   RsyncServiceWrapper* lpInst = dynamic_cast<RsyncServiceWrapper*>(pInst);
   JobStart lRequest;
   JobStartResponse lResponse;

   e.pushFrame(TraceFrame("RsyncServiceWrapper", "push",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->push(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}




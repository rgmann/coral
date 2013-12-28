#include "rpc_getting_started.pb.h"
#include "DatabaseWrapper.h"

using namespace liber::rpc;
using namespace rpc_getting_started;

//-----------------------------------------------------------------------------
DatabaseWrapper::DatabaseWrapper()
:  InstanceWrapper()
{
}

//-----------------------------------------------------------------------------
DatabaseWrapper::~DatabaseWrapper()
{
}

//-----------------------------------------------------------------------------
bool DatabaseWrapper::initialize(const std::string& params)
{
   bool lbSuccess = true;

   /* TODO: Perform instance initialization. */

   return lbSuccess;
}

//-----------------------------------------------------------------------------
bool DatabaseWrapper::destroy(const std::string& params)
{
   bool lbSuccess = true;

   /* TODO: Destroy instance. */

   return lbSuccess;
}

//-----------------------------------------------------------------------------
void DatabaseWrapper::
createDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
   UserAttributes lRequest;
   DbStatus lResponse;

   e.pushFrame(TraceFrame("DatabaseWrapper", "create",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->create(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void DatabaseWrapper::
readDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
   UserName lRequest;
   UserAttributesList lResponse;

   e.pushFrame(TraceFrame("DatabaseWrapper", "read",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->read(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void DatabaseWrapper::
updateDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
   UserAttributes lRequest;
   DbStatus lResponse;

   e.pushFrame(TraceFrame("DatabaseWrapper", "update",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->update(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}


//-----------------------------------------------------------------------------
void DatabaseWrapper::
destroyDelegate
(InstanceWrapper* pInst, const std::string& request, std::string& response, RpcException& e)
{
   DatabaseWrapper* lpInst = dynamic_cast<DatabaseWrapper*>(pInst);
   UserName lRequest;
   DbStatus lResponse;

   e.pushFrame(TraceFrame("DatabaseWrapper", "destroy",
                              __FILE__, __LINE__));

   lRequest.ParseFromString(request);

   if (!lRequest.IsInitialized())
   {
      e.id = MissingParameters;
      e.message = "Parameters not initialized";
      return;
   }

   lpInst->destroy(lRequest, lResponse, e);
   lResponse.SerializeToString(&response);

   e.popFrame();
}




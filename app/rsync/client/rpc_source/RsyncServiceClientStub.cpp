#include "rpc_rsync_service.pb.h"
#include "RsyncServiceClientStub.h"

using namespace liber::rpc;
using namespace rpc_rsync_service;

//------------------------------------------------------------------------------
RsyncServiceClientStub::RsyncServiceClientStub(RpcClient &client)
: RpcClientResource(client, "RsyncService")
{
}

//------------------------------------------------------------------------------
RsyncServiceClientStub::~RsyncServiceClientStub()
{
}

//------------------------------------------------------------------------------
void RsyncServiceClientStub::queryFile(const FileQuery& request, FileQueryResponse& response) throw (RpcException)
{
   if (!call("queryFile", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void RsyncServiceClientStub::pull(const JobStart& request, JobStartResponse& response) throw (RpcException)
{
   if (!call("pull", request, response))
   {
      throw getLastError();
   }
}


//------------------------------------------------------------------------------
void RsyncServiceClientStub::push(const JobStart& request, JobStartResponse& response) throw (RpcException)
{
   if (!call("push", request, response))
   {
      throw getLastError();
   }
}




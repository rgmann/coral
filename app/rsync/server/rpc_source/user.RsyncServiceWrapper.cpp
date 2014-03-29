#include "RsyncServiceWrapper.h"

using namespace liber::rpc;
using namespace rpc_rsync_service;

//-----------------------------------------------------------------------------
void RsyncServiceWrapper::
queryFile(const FileQuery& request, FileQueryResponse& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "RsyncServiceWrapper::queryFile has not been implemented.";
   response.set_file_found(boost::filesystem::exists(request.get_path()));
   if (response.get_file_found())
   {
      response.set_file_size(boost::filesystem::file_size(request.get_path()));
   }
}


//-----------------------------------------------------------------------------
void RsyncServiceWrapper::
pull(const JobStart& request, FileQueryResponse& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "RsyncServiceWrapper::pull has not been implemented.";
   i32 lJobID = mJobManager.addJob(new PullJob(request.get_path()));
   response.set_job_id(lJobID);
}


//-----------------------------------------------------------------------------
void RsyncServiceWrapper::
push(const JobStart& request, FileQueryResponse& response, RpcException& e)
{
   /* TODO: Implement action here and remove the following two lines. */
//   e.id = UnimplementedAction;
//   e.message = "RsyncServiceWrapper::push has not been implemented.";
   i32 lJobID = mJobManager.addJob(new PullJob(request.get_path()));
   response.set_job_id(lJobID);
}




#include "Log.h"
#include "RsyncPacket.h"
#include "RsyncJob.h"
#include "RsyncNode.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
RsyncNode::RsyncNode(
   const boost::filesystem::path& root,
   WorkerGroup& worker_group
)
// :  worker_group_ ( worker_group )
:  job_agent_    ( file_sys_interface_, router_, worker_group )
{
   file_sys_interface_.setRoot(root);

   bool subscribe_success = router_.subscribe(
      RsyncPacket::RsyncJobAgent,
      &job_agent_,
      liber::netapp::kSubscriberModeReadWrite );

   if ( subscribe_success == false )
   {
      liber::log::error(
         "RsyncNode: Failed to register JobAgent as packet subscriber!\n");
   }
}

//----------------------------------------------------------------------------
RsyncNode::~RsyncNode()
{
   router_.unsubscribe( RsyncPacket::RsyncJobAgent, &job_agent_ );
}

//----------------------------------------------------------------------------
void RsyncNode::setJobCompletionCallback( RsyncJobCallback* callback_ptr )
{
   job_agent_.setJobCompletionCallback( callback_ptr );
}

//----------------------------------------------------------------------------
void RsyncNode::unsetJobCompletionCallback()
{
   job_agent_.unsetJobCompletionCallback();
}

//----------------------------------------------------------------------------
void RsyncNode::setJobRequestCallback( JobRequestCallback* callback_ptr )
{
   job_agent_.setJobRequestCallback( callback_ptr );
}

//----------------------------------------------------------------------------
void RsyncNode::unsetJobRequestCallback()
{
   job_agent_.unsetJobRequestCallback();
}

//----------------------------------------------------------------------------
bool RsyncNode::setSegmentSize( ui32 segment_size_bytes )
{
   bool limits_ok = true;

   limits_ok &= ( segment_size_bytes > 0 );
   limits_ok &= ( segment_size_bytes < 65000 );

   if ( limits_ok )
   {
      job_agent_.limits_.segment_size_bytes = segment_size_bytes;
   }

   return limits_ok;
}

//----------------------------------------------------------------------------
bool RsyncNode::setMaximumChunkSize( ui32 maximum_chunk_size_bytes )
{
   bool limits_ok = true;

   limits_ok &= ( maximum_chunk_size_bytes > 0 );
   limits_ok &= ( maximum_chunk_size_bytes < 65000 );

   if ( limits_ok )
   {
      job_agent_.limits_.maximum_chunk_size_bytes = maximum_chunk_size_bytes;
   }

   return limits_ok;
}

//----------------------------------------------------------------------------
bool RsyncNode::setCompletionTimeout( ui32 completion_timeout_ms )
{
   bool limits_ok = true;

   limits_ok &= ( completion_timeout_ms > 0 );

   if ( limits_ok )
   {
      job_agent_.limits_.completion_timeout_ms = completion_timeout_ms;
   }

   return limits_ok;
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::sync(
   const ResourcePath& destination,
   const ResourcePath& source
)
{
   return job_agent_.createJob( destination, source );
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::push( const boost::filesystem::path& filepath )
{
   ResourcePath destination( filepath, true );
   ResourcePath source( filepath, false );

   return job_agent_.createJob( destination, source );
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::pull( const boost::filesystem::path& filepath )
{
   ResourcePath destination( filepath, false );
   ResourcePath source( filepath, true );

   return job_agent_.createJob( destination, source );
}

//----------------------------------------------------------------------------
PacketSubscriber& RsyncNode::subscriber()
{
   return router_;
}


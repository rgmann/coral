#include "Log.h"
#include "RsyncPacket.h"
#include "RsyncJob.h"
#include "RsyncNode.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
RsyncNode::RsyncNode( const boost::filesystem::path& root, WorkerGroup& worker_group )
: worker_group_ ( worker_group )
, job_agent_ ( file_sys_interface_, router_, worker_group )
, segment_size_       ( 256 )
{
  file_sys_interface_.setRoot(root);
  if ( router_.addSubscriber( RsyncPacket::RsyncJobAgent, &job_agent_ ) == false )
    liber::log::error("RsyncNode: Failed to register JobAgent as packet subscriber!\n");
}

//----------------------------------------------------------------------------
RsyncNode::~RsyncNode()
{
  router_.removeSubscriber( RsyncPacket::RsyncJobAgent );
}

//----------------------------------------------------------------------------
void RsyncNode::setCallback( RsyncJobCallback* callback_ptr )
{
  job_agent_.setCallback( callback_ptr );
}

//----------------------------------------------------------------------------
void RsyncNode::unsetCallback()
{
  job_agent_.unsetCallback();
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::sync(
  const boost::filesystem::path& destination_path,
  const boost::filesystem::path& source_path,
  bool remote_destination,
  bool remote_source
)
{
  ResourcePath destination( destination_path, remote_destination );
  ResourcePath source( source_path, remote_source);

  return job_agent_.createJob( destination, source, segment_size_ );
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::push( const boost::filesystem::path& filepath )
{
  ResourcePath destination( filepath, true );
  ResourcePath source( filepath, false );

  return job_agent_.createJob( destination, source, segment_size_ );
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::pull( const boost::filesystem::path& filepath )
{
  ResourcePath destination( filepath, false );
  ResourcePath source( filepath, true );

  return job_agent_.createJob( destination, source, segment_size_ );
}

//----------------------------------------------------------------------------
PacketSubscriber& RsyncNode::subscriber()
{
  return router_;
}


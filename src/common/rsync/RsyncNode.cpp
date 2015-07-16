#include "Log.h"
#include "RsyncPacket.h"
#include "RsyncJob.h"
#include "RsyncNode.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
RsyncNode::RsyncNode( const boost::filesystem::path& root )
: IThread       ( "RsyncNode" )
, callback_ptr_ ( NULL )
, segmenter_    ( file_sys_interface_ )
, authority_    ( file_sys_interface_ )
, assembler_    ( file_sys_interface_ )
, job_agent_    ( file_sys_interface_, segmenter_, authority_, assembler_ )
, authority_service_  ( file_sys_interface_ )
, segment_size_       ( 256 )
{
  file_sys_interface_.setRoot(root);

  router_.addSubscriber(RsyncPacket::RsyncAuthorityService,
                        &authority_service_);
  router_.addSubscriber(RsyncPacket::RsyncAuthorityInterface,
                        &authority_.getSubscriber());
  router_.addSubscriber(RsyncPacket::RsyncJobAgent,
                        &job_agent_);

  segmenter_.launch();
  authority_.launch();
  assembler_.launch();

  launch();
}

//----------------------------------------------------------------------------
RsyncNode::~RsyncNode()
{
  cancel(true);

  segmenter_.cancel(true);
  authority_.cancel(true);
  assembler_.cancel(true);

  router_.removeSubscriber( RsyncPacket::RsyncAuthorityService );
  router_.removeSubscriber( RsyncPacket::RsyncAuthorityInterface );
  router_.removeSubscriber( RsyncPacket::RsyncJobAgent );
}

//----------------------------------------------------------------------------
void RsyncNode::setCallback( RsyncJobCallback* callback_ptr )
{
  callback_lock_.lock();
  callback_ptr_ = callback_ptr;
  callback_lock_.unlock();
}

//----------------------------------------------------------------------------
void RsyncNode::unsetCallback()
{
  callback_lock_.lock();
  callback_ptr_ = NULL;
  callback_lock_.unlock();
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
PacketSubscriber& RsyncNode::subscriber()
{
  return router_;
}

//----------------------------------------------------------------------------
void RsyncNode::run(const bool& bShutdown)
{
  while ( bShutdown == false )
  {
    RsyncJob* job_ptr = NULL;

    if ( ( job_ptr = job_agent_.nextJob() ) != NULL )
    {
      if ( job_ptr->waitDone() == false )
      {
        log::error("RsyncNode - Timeout waiting for %s job to finish.\n",
                   job_ptr->descriptor().getSource().path.string().c_str());
      }

      callback_lock_.lock();
      if ( callback_ptr_ )
      {
        callback_ptr_->call( job_ptr->descriptor(), job_ptr->report() );
      }
      callback_lock_.unlock();

      job_agent_.releaseJob( job_ptr );
    }
  }
}


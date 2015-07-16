#include "Log.h"
#include "RsyncJob.h"
#include "AuthorityThread.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AuthorityThread::AuthorityThread( FileSystemInterface& file_sys_interface )
: IThread         ( "AuthorityThread" )
, local_authority_( file_sys_interface )
{
  job_queue_.initialize();
}

//----------------------------------------------------------------------------
AuthorityThread::~AuthorityThread()
{
}

//----------------------------------------------------------------------------
void AuthorityThread::addJob( RsyncJob* job_ptr )
{
  if ( job_ptr )
  {
    job_queue_.push( job_ptr );
  }
}

//----------------------------------------------------------------------------
PacketSubscriber& AuthorityThread::getSubscriber()
{
  return remote_authority_;
}

//----------------------------------------------------------------------------
void AuthorityThread::setRequestID( int request_id )
{
  remote_authority_.setRequestID( request_id );
}

//----------------------------------------------------------------------------
void AuthorityThread::run(const bool& bShutdown)
{
  while ( !bShutdown )
  {
    RsyncJob* job_ptr = NULL;

    // Wait for a job. This is also a thread cancellation point.
    if ( job_queue_.pop( job_ptr ) && job_ptr )
    {
      if ( job_ptr->descriptor().getSource().remote )
      {
        remote_authority_.process( job_ptr );
      }
      else
      {
        local_authority_.process( job_ptr );
      }

      job_ptr->signalAuthDone();
    }
  }
}


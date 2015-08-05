#include "Log.h"
#include "RsyncJob.h"
#include "AuthorityThread.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AuthorityThread::AuthorityThread()
: IThread         ( "AuthorityThread" )
{
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
        liber::log::debug("AuthorityThread::run: Processing local job\n");
        local_authority_.process( job_ptr );
        liber::log::debug("AuthorityThread::run: Finished processing local job\n");
      }

      job_ptr->signalAuthDone();
    }
  }
}


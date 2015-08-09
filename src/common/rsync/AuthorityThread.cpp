#include "Log.h"
#include "RsyncJob.h"
#include "AuthorityThread.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AuthorityThread::AuthorityThread( JobAgentPairQueue& queue )
: IThread         ( "AuthorityThread" )
, job_queue_( queue )
{
}

//----------------------------------------------------------------------------
AuthorityThread::~AuthorityThread()
{
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
    JobAgentPair job_agent_pair;

    // Wait for a job. This is also a thread cancellation point.
    if ( job_queue_.pop( job_agent_pair ) )
    {
      JobAgent* agent_ptr = job_agent_pair.first;
      RsyncJob* job_ptr = job_agent_pair.second;
      if ( agent_ptr && job_ptr )
      {
        if ( job_ptr->descriptor().getSource().remote() )
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
}


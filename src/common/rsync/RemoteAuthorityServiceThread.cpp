#include "Log.h"
#include "RsyncJob.h"
#include "JobAgent.h"
#include "RemoteAuthorityServiceThread.h"

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
RemoteAuthorityServiceThread::RemoteAuthorityServiceThread( JobAgentPairQueue& queue )
: IThread( "RemoteAuthorityServiceThread" )
, job_queue_( queue )
{
}

//-----------------------------------------------------------------------------
RemoteAuthorityServiceThread::~RemoteAuthorityServiceThread()
{
}

//-----------------------------------------------------------------------------
void RemoteAuthorityServiceThread::run( const bool& shutdown )
{
  while ( shutdown == false )
  {
    JobAgentPair job_agent_pair;

    if ( job_queue_.pop( job_agent_pair ) )
    {
      JobAgent* agent_ptr = job_agent_pair.first;
      RsyncJob* job_ptr = job_agent_pair.second;

      if ( agent_ptr && job_ptr )
      {
        authority_.process( job_ptr );

        agent_ptr->releaseJob( job_ptr );
      }
    }
  }

  log::debug("RemoteAuthorityServiceThread: Shutting down\n");
}

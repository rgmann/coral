#include "RsyncJobCallback.h"
#include "Worker.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
Worker::Worker( Queue<JobAgentPair>& job_queue )
: IThread( "RsyncWorker")
, job_queue_( job_queue )
{
  segmenter_.launch();
  authority_.launch();
  assembler_.launch();

  launch();
}

//-----------------------------------------------------------------------------
Worker::~Worker()
{
  cancel(true);

  segmenter_.cancel(true);
  authority_.cancel(true);
  assembler_.cancel(true);
}

//-----------------------------------------------------------------------------
void Worker::run( const bool& shutdown )
{
  while ( shutdown == false )
  {
    std::pair<JobAgent*, RsyncJob*> job;

    if ( job_queue_.pop( job ) )
    {
      RsyncJob* job_ptr = job.second;
      JobAgent* agent_ptr = job.first;

      liber::log::debug("Worker::run: Received job\n");

      if ( job_ptr )
      {
        if ( job_ptr->descriptor().getDestination().remote == false )
        {
          liber::log::debug("Worker::run PIPELINE\n");
          // If the job was successfully created, and the destination path is
          // local, add the job to the local pipeline.
          segmenter_.addJob( job_ptr );
          authority_.addJob( job_ptr );
          assembler_.addJob( job_ptr );

          if ( job_ptr->waitDone() == false )
          {
            log::error("RsyncNode - Timeout waiting for %s job to finish.\n",
                       job_ptr->descriptor().getSource().path.string().c_str());
          }

          liber::log::debug("Worker::run: Released job\n");
        }
        else if ( job_ptr->descriptor().isRemoteRequest() )
        {
          liber::log::debug("Worker::run REMOTE AUTH SERVICE\n");
          // authority_service_.addJob( job_ptr );
          authority_service_.process( job_ptr );
          liber::log::debug("Worker::run REMOTE AUTH DONE\n");
        }

        // if ( job_ptr->waitDone() == false )
        // {
        //   log::error("RsyncNode - Timeout waiting for %s job to finish.\n",
        //              job_ptr->descriptor().getSource().path.string().c_str());
        // }

        agent_ptr->releaseJob( job_ptr );
      }
      else
      {
        liber::log::debug("Worker::run: NULL job pointer\n");
      }
    }
  }
}
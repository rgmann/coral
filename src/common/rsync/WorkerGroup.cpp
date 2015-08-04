#include "WorkerGroup.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
WorkerGroup::WorkerGroup( int worker_count )
{
  for ( int count = 0; count < worker_count; ++count )
  {
    workers_.push_back( new Worker( job_queue_ ) );
  }
}

//-----------------------------------------------------------------------------
WorkerGroup::~WorkerGroup()
{
  std::vector<Worker*>::iterator worker = workers_.begin();
  for (; worker != workers_.end(); ++worker )
  {
    if ( *worker )
    {
      delete *worker;
      *worker = NULL;
    }
  }

  workers_.clear();
}

//-----------------------------------------------------------------------------
RsyncError WorkerGroup::addJob( JobAgent* agent_ptr, RsyncJob* job_ptr )
{
  RsyncError add_status = kRsyncNoWorkersAvailable;

  if ( workers_.size() > 0 )
  {
    liber::log::debug("WorkerGroup::addJob: Adding job\n");
    job_queue_.push( std::make_pair( agent_ptr, job_ptr ) );

    add_status = RsyncSuccess;
  }

  return add_status;
}

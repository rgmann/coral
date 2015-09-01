#include "WorkerGroup.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
WorkerGroup::WorkerGroup( int worker_count )
{
   for ( int count = 0; count < worker_count; ++count )
   {
      Worker* worker = new (std::nothrow) Worker(
         segment_queue_,
         local_auth_queue_,
         assembly_queue_,
         remote_auth_queue_
      );

      if ( worker )
      {
         workers_.push_back( worker );
      }
   }
}

//-----------------------------------------------------------------------------
WorkerGroup::~WorkerGroup()
{
   std::vector<Worker*>::iterator worker = workers_.begin();
   for (; worker != workers_.end(); ++worker )
   {
      if ( *worker != NULL )
      {
         delete *worker;
         *worker = NULL;
      }
   }
}

//-----------------------------------------------------------------------------
RsyncError WorkerGroup::addJob( JobAgent* agent_ptr, RsyncJob* job_ptr )
{
   RsyncError add_status = kRsyncNoWorkersAvailable;

   if ( workers_.size() > 0 )
   {
      JobAgentPair job_agent_pair = std::make_pair( agent_ptr, job_ptr );

      if ( job_ptr->descriptor().getDestination().remote() == false )
      {
         segment_queue_.push( job_agent_pair );
         local_auth_queue_.push( job_agent_pair );
         assembly_queue_.push( job_agent_pair );
      }
      else
      {
         remote_auth_queue_.push( job_agent_pair );
      }

      add_status = kRsyncSuccess;
   }

   return add_status;
}

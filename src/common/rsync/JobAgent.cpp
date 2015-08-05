#include <new>
#include "Log.h"
#include "RsyncPacket.h"
#include "FileSystemInterface.h"
#include "RsyncJob.h"
#include "RemoteJobResult.h"
#include "RsyncQueryResponse.h"
#include "WorkerGroup.h"
#include "RsyncJobCallback.h"
#include "JobAgent.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

typedef boost::uuids::uuid buuid;

//----------------------------------------------------------------------------
JobAgent::
JobAgent(
  FileSystemInterface& file_sys_interface, 
  RsyncPacketRouter&   packet_router,
  WorkerGroup&         worker_group
)
: PacketSubscriber()
, file_sys_interface_ (file_sys_interface)
, packet_router_ ( packet_router )
, worker_group_  ( worker_group )
, create_destination_stub_( true )
, callback_ptr_( NULL )
{
}

//----------------------------------------------------------------------------
JobAgent::~JobAgent()
{
}

//----------------------------------------------------------------------------
void JobAgent::setCallback( RsyncJobCallback* callback_ptr )
{
  boost::mutex::scoped_lock guard( callback_lock_ );
  callback_ptr_ = callback_ptr;
}
//----------------------------------------------------------------------------
void JobAgent::unsetCallback()
{
  boost::mutex::scoped_lock guard( callback_lock_ );
  callback_ptr_ = NULL;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::createJob(
  const ResourcePath& destination,
  const ResourcePath& source,
  ui32 segment_size
)
{
  RsyncError job_create_status = RsyncBadDescriptor;

  RsyncJob* job_ptr = new (std::nothrow) RsyncJob(
    file_sys_interface_,
    packet_router_
  );

  if ( job_ptr )
  {
    JobDescriptor& descriptor = job_ptr->descriptor();

    descriptor.setDestination( destination );
    descriptor.setSource( source );
    descriptor.setSegmentSize( segment_size );

    if ( ( job_create_status = createJob( job_ptr ) ) != RsyncSuccess )
    {
      delete job_ptr;
    }
  }
  else
  {
    log::error("JobAgent::createJob - Failed to allocated RsyncJob\n");
  }

  return job_create_status;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::createJob( const char* data_ptr, ui32 size_bytes )
{
  RsyncError job_create_status = RsyncSuccess;

  RsyncJob* job_ptr = new (std::nothrow) RsyncJob(
    file_sys_interface_,
    packet_router_
  );

  if ( job_ptr )
  {
    JobDescriptor& descriptor = job_ptr->descriptor();

    if ( descriptor.deserialize( data_ptr, size_bytes ) )
    {
      // Indicate that the job was remotely requested.
      descriptor.setRemoteRequest();

      // Received jobs can only specify the following combinations (table is
      // from the perspective of the RsyncNode::sync call):
      //
      // Source |  Dest  | Description
      // ---------------------------------------------------------
      // remote | remote | Tell remote node to synchronize two of its resources.
      // remote | local  | Not allowed; performed locally.
      // local  | remote | Tell remote not to synchronize its resource to local resource.
      // local  | local  | Not allowed; performed locally.
      if ( descriptor.getDestination().remote )
      {
        descriptor.getDestination().remote = false;

        // If a remote JobAgent specifies a remote source, the source is now
        // local (and vice versa).
        descriptor.getSource().remote = (descriptor.getSource().remote == false);

        // Create the job.
        if ( ( job_create_status = createJob( job_ptr ) ) != RsyncSuccess)
        {
          delete job_ptr;
        }
      }
      else
      {
        log::error("JobAgent::createJob - Bad remote job\n");
        job_create_status = error(job_ptr, RsyncBadRemoteJob);
      }
    }
    else
    {
      log::error("JobAgent::createJob - Bad descriptor\n");
      job_create_status = error(job_ptr, RsyncBadDescriptor);
    }
  }
  else
  {
    log::error("JobAgent::createJob - Failed to allocate RsyncJob\n");
  }

  return job_create_status;
}

//---------------------------------------------------------------------------
RsyncError JobAgent::createJob( RsyncJob* job_ptr )
{
  RsyncError job_create_status = RsyncSuccess;

  JobDescriptor& descriptor = job_ptr->descriptor();

  // If the source is local, verify that it exists.  If it does not, job
  // creation failed.
  if ( descriptor.getSource().remote == false )
  {
    if ( file_sys_interface_.exists( descriptor.getSourcePath() ) == false )
    {
      log::error("JobAgent::createJob - Source resource not found\n");
      job_create_status = error( job_ptr, RsyncSourceFileNotFound );
    }
  }

  // If the destination is local, verify that it exists. If it does not, but
  // stub creation is enabled, attempt to create it with touch. If stub
  // creation is not enabled, job creation failed.
  if ( job_create_status == RsyncSuccess )
  {
    if ( descriptor.getDestination().remote == false )
    {
      // If the destination file does not exist, create it (if so configured).
      if ( create_destination_stub_ )
      {
        if ( file_sys_interface_.touch( descriptor.getDestinationPath() ) == false )
        {
          log::error("JobAgent::createJob - Failed to touch destination resource\n");
          job_create_status = error(job_ptr, RsyncDestinationFileNotFound);
        }
      }
      else
      {
        // If the destination still does not exist, the job cannot be performed.
        if ( file_sys_interface_.exists( descriptor.getDestinationPath() ) == false )
        {
          log::error("JobAgent::createJob - Destination resource not found\n");
          job_create_status = error(job_ptr, RsyncDestinationFileNotFound);
        }
      }
    }
  }

  if ( job_create_status == RsyncSuccess )
  {
    if ( ( job_create_status = addActiveJob( job_ptr ) ) == RsyncSuccess )
    {
      // If this is a remote job, send the descriptor to the remote node.
      if ( descriptor.getDestination().remote )
      {
        job_create_status = sendRemoteJob( descriptor );
      }
    }
  }

  //
  // Update status on the the job.
  //
  // job_create_status = error( job_ptr, job_create_status );
  error( job_ptr, job_create_status );

  return job_create_status;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::addActiveJob( RsyncJob* job_ptr )
{
  RsyncError job_create_status = RsyncSuccess;

  std::pair<std::map<buuid, RsyncJob*>::iterator,bool> insert_status;
  std::pair<buuid, RsyncJob*> job_pair;

  job_pair = std::make_pair( job_ptr->descriptor().uuid(), job_ptr );
  insert_status = active_jobs_.insert( job_pair );

  // If the job was successfully added to the active-job table,
  // add the job to the ready job queue.
  if ( insert_status.second )
  {
    if ( job_ptr->descriptor().getDestination().remote == false )
    {
      worker_group_.addJob( this, job_ptr );
      liber::log::debug("JobAgent::addActiveJob: ADDED JOB  %d\n", job_ptr->descriptor().isRemoteRequest());
    }

    if ( job_create_status != RsyncSuccess )
    {
      active_jobs_.erase( insert_status.first );
    }
  }
  else
  {
    log::error("JobAgent::addActiveJob - "
                   "Failed to add job to active job table\n");
    job_create_status = RsyncJobTableInsertionError;
  }

  return job_create_status;
}

//----------------------------------------------------------------------------
void JobAgent::removeActiveJob( RsyncJob* job_ptr )
{
  // Remove the job from the active job table.
  if ( active_jobs_.count( job_ptr->descriptor().uuid() ) > 0 )
  {
    active_jobs_.erase( job_ptr->descriptor().uuid() );
  }
  else
  {
    log::warn("JobAgent::removeActiveJob - Specified job not found\n");
  }
}

//----------------------------------------------------------------------------
RsyncError JobAgent::sendRemoteJob( const JobDescriptor& descriptor )
{
  liber::log::debug("JobAgent::sendRemoteJob\n");
  // Send the remote job request.
  sendPacketTo(
    RsyncPacket::RsyncJobAgent,
    new RsyncPacket( RsyncPacket::RsyncJobRequest, descriptor.serialize() )
  );

  return RsyncSuccess;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::error( RsyncJob* job_ptr, RsyncError error )
{
  job_ptr->report().createJobStatus = error;

  // 
  if ( job_ptr->descriptor().isRemoteRequest() && ( error != RsyncSuccess ) )
  {
    RemoteJobResult result( job_ptr->descriptor().uuid(), job_ptr->report() );

    sendPacketTo(
      RsyncPacket::RsyncJobAgent, 
      new RsyncPacket( RsyncPacket::RsyncJobComplete, result.serialize() )
    );
  }

  return error;
}

//----------------------------------------------------------------------------
void JobAgent::releaseJob( RsyncJob* job_ptr )
{
  JobDescriptor& descriptor = job_ptr->descriptor();

  // Notify the remote node that a job has been completed.
  if ( descriptor.isRemoteRequest() )
  {
    RemoteJobResult result( descriptor.uuid(), job_ptr->report() );

    sendPacketTo( RsyncPacket::RsyncJobAgent, new RsyncPacket(
      RsyncPacket::RsyncJobComplete, result.serialize()
    ));
  }

  // Remote the job from the active job table.
  removeActiveJob( job_ptr );

  {
    boost::mutex::scoped_lock guard( callback_lock_ );

    if ( callback_ptr_ )
    {
      callback_ptr_->call( job_ptr->descriptor(), job_ptr->report() );
    }
  }

  // Delete the job.
  delete job_ptr;
  job_ptr = NULL;
}

//----------------------------------------------------------------------------
bool JobAgent::put( const char* data_ptr, ui32 size_bytes )
{
  bool route_success = true;

  RsyncPacket packet;

  if ( packet.unpack( data_ptr, size_bytes ) )
  {
    switch ( packet.data()->type )
    {
      case RsyncPacket::RsyncJobRequest:
        createJob( (const char*)packet.dataPtr(), packet.data()->length );
        break;

      case RsyncPacket::RsyncJobComplete:
        finishRemoteJob( (const char*)packet.dataPtr(), packet.data()->length );
        break;

      case RsyncPacket::RsyncRemoteAuthQuery:
        handleRemoteJobRequest( packet.dataPtr(), packet.data()->length);
        break;

      default:
        log::error("JobAgent - Unrecognized packet type");
        route_success = false;
        break;
    }
  }

  return route_success;
}

//-----------------------------------------------------------------------------
void JobAgent::handleRemoteJobRequest( const void* pData, ui32 nLength )
{
  liber::log::debug("JobAgent::handleRemoteJobRequest\n");
  RsyncJob* job_ptr = new (std::nothrow) RsyncJob(
    file_sys_interface_,
    packet_router_
  );

  if ( job_ptr )
  {
    JobDescriptor& descriptor = job_ptr->descriptor();

    if ( descriptor.deserialize((const char*)pData, nLength) )
    {
      RsyncError status = RsyncSuccess;

      // Indicate that the job was remotely requested.
      descriptor.setRemoteRequest();

      // This is an auth request, so by definition the destination must be remote.
      descriptor.getDestination().remote = true;

      // if (mpUserHandler)
      // {
      //   status = mpUserHandler(descriptor);
      // }
      // else
      {
        // status = defaultQueryHandler(descriptor);
        if (job_ptr->fileSystem().exists(job_ptr->descriptor().getSource().path) == false)
        {
          status = RsyncSourceFileNotFound;
          log::error("Remote job query failed for %s\n",
                     job_ptr->descriptor().getSource().path.string().c_str());
        }
      }

      //
      // If a job was successfully created, add it to the job queue.
      if ( status == RsyncSuccess )
      {
        bool add_success = true;

        // If the job is not already in the active job table, add it now.
        // The job will already be in the table if this job was initiated with
        // a push operation.
        if ( active_jobs_.count( job_ptr->descriptor().uuid() ) == 0 )
        {
          std::pair<std::map<buuid, RsyncJob*>::iterator,bool> insert_status;
          std::pair<buuid, RsyncJob*> job_pair;

          job_pair = std::make_pair( job_ptr->descriptor().uuid(), job_ptr );
          insert_status = active_jobs_.insert( job_pair );

          add_success = insert_status.second;
        }

        // mJobQueue.push( job_ptr );
        if ( add_success )
        {
          status = worker_group_.addJob( this, job_ptr );
          liber::log::debug("JobAgent::handleRemoteJobRequest: ADDED JOB %d\n", job_ptr->descriptor().isRemoteRequest());
        }
        else
        {
          liber::log::debug("JobAgent::handleRemoteJobRequest: FAILED TO ADD JOB\n");
        }
      }
      // Respond immediately on error.
      else
      {
        RsyncQueryResponse response( descriptor.uuid(), status );

        sendPacketTo( RsyncPacket::RsyncAuthorityInterface, new (std::nothrow) RsyncPacket(
          RsyncPacket::RsyncRemoteAuthAcknowledgment,
          response.serialize()
        ));
      }
    }
    else
    {
      log::error("RemoteAuthorityService::handleRemoteJobRequest - "
                 "Failed to deserialize JobDescriptor\n");
      delete job_ptr;
    }
  }
  else
  {
    log::error("RemoteAuthorityService::handleRemoteJobRequest - "
               "Failed to allocate RsyncJob\n");
  }
}

//----------------------------------------------------------------------------
void JobAgent::finishRemoteJob( const char* data_ptr, ui32 size_bytes )
{
  RemoteJobResult job_result;

  if ( job_result.deserialize( data_ptr,  size_bytes ) )
  {
    if ( active_jobs_.count( job_result.uuid() ) > 0 )
    {
      std::map<buuid, RsyncJob*>::iterator job_iterator;
      job_iterator = active_jobs_.find( job_result.uuid() );

      if ( job_iterator->second )
      {
        if ( job_iterator->second->descriptor().getDestination().remote )
        {
          job_iterator->second->mergeReport(job_result.report());
          job_iterator->second->signalAllDone();
        }
      }
      else
      {
        log::error("JobAgent::finishRemoteJob - NULL job\n");
      }
    }
    else
    {
      log::error("JobAgent::finishRemoteJob - Failed to find job with specified UUID\n");
    }
  }
  else
  {
    log::error("JobAgent::finishRemoteJob - Failed to deserialize job status\n");
  }
}


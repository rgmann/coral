#include <new>
#include "Log.h"
#include "RsyncPacket.h"
#include "FileSystemInterface.h"
#include "RsyncJob.h"
#include "JobAgent.h"
#include "RemoteJobResult.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

typedef boost::uuids::uuid buuid;

//----------------------------------------------------------------------------
JobAgent::
JobAgent(
  FileSystemInterface& file_sys_interface, 
  SegmenterThread& segmenter,
  AuthorityThread& authority,
  AssemblerThread& assembler
)
: PacketSubscriber()
, file_sys_interface_ (file_sys_interface)
, segmenter_          ( segmenter )
, authority_          ( authority )
, assembler_          ( assembler )
, create_destination_stub_( true )
{
}

//----------------------------------------------------------------------------
JobAgent::~JobAgent()
{
}

//----------------------------------------------------------------------------
RsyncJob* JobAgent::nextJob()
{
  RsyncJob* job_ptr = NULL;
  ready_jobs_.pop( job_ptr );
  return job_ptr;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::createJob(
  const ResourcePath& destination,
  const ResourcePath& source,
  ui32 segment_size
)
{
  RsyncError job_create_status = RsyncBadDescriptor;
  RsyncJob*  job_ptr   = NULL;

  if ( (job_ptr = new (std::nothrow) RsyncJob() ) )
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
  RsyncJob*  job_ptr   = NULL;

  if ( ( job_ptr = new (std::nothrow) RsyncJob() ) )
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
      // If the job was successfully created, and the destination path is
      // local, add the job to the local pipeline.
      segmenter_.addJob( job_ptr );
      authority_.addJob( job_ptr );
      assembler_.addJob( job_ptr );
    }

    if ( ready_jobs_.push( job_ptr ) == false )
    {
      log::error("JobAgent::addActiveJob - Failed to add job to queue\n");
      job_create_status = RsyncQueueError;
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
RsyncError JobAgent::sendRemoteJob(const JobDescriptor& descriptor)
{
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

    sendPacketTo(
      RsyncPacket::RsyncJobAgent,
      new RsyncPacket( RsyncPacket::RsyncJobComplete, result.serialize() )
    );
  }

  // Remote the job from the active job table.
  removeActiveJob( job_ptr );

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

    default:
      log::error("JobAgent - Unrecognized packet type");
      route_success = false;
      break;
    }
  }

  return route_success;
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
        job_iterator->second->mergeReport(job_result.report());
        job_iterator->second->signalAllDone();
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


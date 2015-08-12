#include <new>
#include "Log.h"
#include "RsyncPacket.h"
#include "FileSystemInterface.h"
#include "RsyncJob.h"
#include "RemoteJobResult.h"
#include "RsyncQueryResponse.h"
#include "WorkerGroup.h"
#include "RsyncJobCallback.h"
#include "RsyncPacketRouter.h"
#include "JobAgent.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

typedef boost::uuids::uuid buuid;

//----------------------------------------------------------------------------
JobAgent::JobAgent(
  FileSystemInterface& file_sys_interface, 
  RsyncPacketRouter&   packet_router,
  WorkerGroup&         worker_group
)
: PacketSubscriber()
, file_sys_interface_ (file_sys_interface)
, packet_router_      ( packet_router )
, worker_group_       ( worker_group )
, create_destination_stub_( true )
, callback_ptr_       ( NULL )
, job_request_callback_ptr_( &default_job_request_callback_ )
{
}

//----------------------------------------------------------------------------
JobAgent::~JobAgent()
{
   // Release all active jobs.
   std::map<buuid, RsyncJob*>::iterator job_iterator = active_jobs_.begin();
   for (; job_iterator != active_jobs_.end(); ++job_iterator )
   {
      RsyncJob* job_ptr = job_iterator->second;

      if ( job_ptr )
      {
         releaseJob( job_ptr );
      }
   }

   active_jobs_.clear();
}

//----------------------------------------------------------------------------
void JobAgent::setJobCompletionCallback( RsyncJobCallback* callback_ptr )
{
   boost::mutex::scoped_lock guard( callback_lock_ );
   callback_ptr_ = callback_ptr;
}
//----------------------------------------------------------------------------
void JobAgent::unsetJobCompletionCallback()
{
   boost::mutex::scoped_lock guard( callback_lock_ );
   callback_ptr_ = NULL;
}

//----------------------------------------------------------------------------
void JobAgent::setJobRequestCallback( JobRequestCallback* callback_ptr )
{
   boost::mutex::scoped_lock guard( callback_lock_ );
   job_request_callback_ptr_ = callback_ptr;
}

//----------------------------------------------------------------------------
void JobAgent::unsetJobRequestCallback()
{
   boost::mutex::scoped_lock guard( callback_lock_ );
   job_request_callback_ptr_ = &default_job_request_callback_;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::createJob(
  const ResourcePath& destination,
  const ResourcePath& source
)
{
   RsyncError job_create_status = kRsyncAllocationError;

   RsyncJob* job_ptr = new (std::nothrow) RsyncJob(
      file_sys_interface_,
      packet_router_
   );

   if ( job_ptr )
   {
      JobDescriptor& descriptor = job_ptr->descriptor();

      descriptor.setDestination( destination );
      descriptor.setSource( source );
      // descriptor.setSegmentSize( segment_size );
      descriptor.setLimits( limits_ );

      job_create_status = createJob( job_ptr );

      // If createJob returns an error, the job was not added to the pipeline.
      // Deallocate the job now.
      if ( job_create_status != RsyncSuccess )
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
   RsyncError job_create_status = kRsyncAllocationError;

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
         // local  | remote | Tell remote node to synchronize its resource to local resource.
         // local  | local  | Not allowed; performed locally.
         if ( descriptor.getDestination().remote() )
         {
            descriptor.getDestination().setRemote( false );

            // If a remote JobAgent specifies a remote source, the source is
            // now local (and vice versa).
            descriptor.getSource().setRemote(
               ( descriptor.getSource().remote() == false )
            );

            job_create_status = createJob( job_ptr );

            // If createJob returns an error, the job was not added to the
            // pipeline. Deallocate the job now.
            if ( job_create_status != RsyncSuccess)
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
         job_create_status = error( job_ptr, kRsyncBadDescriptor );
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

   {
      boost::mutex::scoped_lock guard( callback_lock_ );

      job_create_status = job_request_callback_ptr_->canFulfill(
         job_ptr->fileSystem(),
         job_ptr->descriptor()
      );
   }

   if ( job_create_status != RsyncSuccess )
   {
      job_create_status = error( job_ptr, job_create_status );
   }

   // If the source or destination is remote, check that the RsyncPacketRouter
   // has been registered with a packet router and that the JobAgent has been
   // registered with the RsyncPacketRouter.
   if ( job_create_status == RsyncSuccess )
   {
      if ( descriptor.getSource().remote() || descriptor.getDestination().remote() )
      {
         if ( ( packet_router_.isSubscribed() == false ) ||
              ( this->isSubscribed() == false ) )
         {
            log::error("JobAgent::createJob - Packet subscription not initialized. Can't complete remote job.\n");
            job_create_status = error( job_ptr, kRsyncNotSubscribed );
         }
      }
   }

   // If the destination is local, verify that it exists. If it does not, but
   // stub creation is enabled, attempt to create it with touch. If stub
   // creation is not enabled, job creation failed.
   if ( job_create_status == RsyncSuccess )
   {
      if ( descriptor.getDestination().remote() == false )
      {
         boost::mutex::scoped_lock guard( callback_lock_ );
         // If the destination file does not exist, create it (if so configured).
         // if ( create_destination_stub_ )
         if ( job_request_callback_ptr_->canTouchDestination() )
         {
            if ( file_sys_interface_.touch( descriptor.getDestinationPath() ) == false )
            {
               log::error("JobAgent::createJob - Failed to touch destination resource\n");
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
         if ( descriptor.getDestination().remote() )
         {
            job_create_status = sendRemoteJob( descriptor );
         }
      }
   }

   //
   // Update status on the the job.
   //
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
      if ( job_ptr->descriptor().getDestination().remote() == false )
      {
         worker_group_.addJob( this, job_ptr );
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
      job_create_status = kRsyncJobTableInsertError;
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
   RsyncError send_status = RsyncSuccess;

   RsyncPacket* packet_ptr = new (std::nothrow) RsyncPacket(
      RsyncPacket::RsyncJobRequest, descriptor.serialize()
   );

   // Attempt to send the request. Failure means that the packet was not
   // added to the send queue, so delete it.
   if ( sendPacketTo( RsyncPacket::RsyncJobAgent, packet_ptr ) == false )
   {
      send_status = kRsyncCommError;
      delete packet_ptr;
   }

   return send_status;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::error( RsyncJob* job_ptr, RsyncError error )
{
   job_ptr->report().createJobStatus = error;

   // 
   if ( job_ptr->descriptor().isRemoteRequest() && ( error != RsyncSuccess ) )
   {
      RemoteJobResult result( job_ptr->descriptor().uuid(), job_ptr->report() );

      // Report the error to the remote node.
      RsyncPacket* packet_ptr = new (std::nothrow) RsyncPacket(
         RsyncPacket::RsyncJobComplete,
         result.serialize()
      );

      if ( packet_ptr )
      {
         if ( sendPacketTo( RsyncPacket::RsyncJobAgent, packet_ptr ) == false )
         {
            error = kRsyncCommError;
            delete packet_ptr;
         }
      }
      else
      {
         error = kRsyncAllocationError;
      }
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

      RsyncPacket* packet_ptr = new (std::nothrow) RsyncPacket(
         RsyncPacket::RsyncJobComplete,
         result.serialize()
      );

      if ( packet_ptr )
      {
         if ( sendPacketTo( RsyncPacket::RsyncJobAgent, packet_ptr ) == false )
         {
            delete packet_ptr;
         }
      }
   }

   // Remote the job from the active job table.
   removeActiveJob( job_ptr );

   // Invoke the job-completion callback (if available).
   {
      boost::mutex::scoped_lock guard( callback_lock_ );

      if ( callback_ptr_ )
      {
         callback_ptr_->call( job_ptr->descriptor(), job_ptr->report() );
      }
   }

   // Finally, the job can be deallocated.
   delete job_ptr;
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
   else
   {
      route_success = false;
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
         descriptor.getDestination().setRemote( true );

         // If a remote JobAgent specifies a remote source, the source is
         // now local (and vice versa).
         descriptor.getSource().setRemote( false );

         {
            boost::mutex::scoped_lock guard( callback_lock_ );

            status = job_request_callback_ptr_->canFulfill(
               job_ptr->fileSystem(),
               job_ptr->descriptor()
            );
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

            if ( add_success )
            {
               status = worker_group_.addJob( this, job_ptr );
            }
            else
            {
               log::error("JobAgent::handleRemoteJobRequest: FAILED TO ADD JOB\n");
               status = kRsyncJobTableInsertError;
            }
         }
         // Respond immediately on error.
         else
         {
            RsyncQueryResponse response( descriptor.uuid(), status );

            RsyncPacket* packet_ptr = new (std::nothrow) RsyncPacket(
               RsyncPacket::RsyncRemoteAuthAcknowledgment,
               response.serialize()
            );

            if ( sendPacketTo( RsyncPacket::RsyncAuthorityInterface, packet_ptr ) == false )
            {
               log::error(
                  "%s - Failed to send remote auth request ack.\n",
                  BOOST_CURRENT_FUNCTION );

               delete packet_ptr;
            }
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
         RsyncJob* job_ptr = job_iterator->second;

         if ( job_ptr )
         {
            if ( job_ptr->descriptor().getDestination().remote() )
            {
               job_ptr->mergeReport( job_result.report() );
               job_ptr->signalAllDone();
            }
         }
         else
         {
            log::error( "%s - NULL job\n", BOOST_CURRENT_FUNCTION );
         }
      }
      else
      {
         log::error(
            "%s - Failed to find job with specified UUID\n",
            BOOST_CURRENT_FUNCTION );
      }
   }
   else
   {
      log::error(
         "%s - Failed to deserialize job status\n",
         BOOST_CURRENT_FUNCTION );
   }
}


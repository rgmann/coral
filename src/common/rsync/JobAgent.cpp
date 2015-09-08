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
:  RsyncPacketSubscriber( false )
,  file_sys_interface_ (file_sys_interface)
,  packet_router_      ( packet_router )
,  worker_group_       ( worker_group )
,  create_destination_stub_( true )
,  callback_ptr_       ( NULL )
,  job_request_callback_ptr_( &default_job_request_callback_ )
,  allow_new_jobs_( true )
{
}

//----------------------------------------------------------------------------
JobAgent::~JobAgent()
{
   disablePacketRouting();

   boost::mutex::scoped_lock guard( active_jobs_lock_ );

   allow_new_jobs_ = false;

   // Release all active jobs.
   JobTable::iterator job_iterator = active_jobs_.begin();
   for (; job_iterator != active_jobs_.end(); ++job_iterator )
   {
      RsyncJob* job_ptr = job_iterator->second;

      if ( job_ptr )
      {
         job_ptr->waitDone();
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
      descriptor.setLimits( limits_ );

      job_create_status = createJob( job_ptr );

      // If createJob returns an error, the job was not added to the pipeline.
      // Deallocate the job now.
      if ( job_create_status != kRsyncSuccess )
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
         // ---------------------------------------------------------------
         // remote | remote | Tell remote node to synchronize two of its
         //        |        | resources.
         // ---------------------------------------------------------------
         // remote | local  | Not allowed; performed locally.
         //        |        |
         // ---------------------------------------------------------------
         // local  | remote | Tell remote node to synchronize its resource
         //        |        | to local resource.
         // ---------------------------------------------------------------
         // local  | local  | Not allowed; performed locally.
         //        |        |
         // ---------------------------------------------------------------
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
            if ( job_create_status != kRsyncSuccess )
            {
               delete job_ptr;
            }
         }
         else
         {
            log::error("JobAgent::createJob - Bad remote job: %s\n",
               descriptor.getDestinationPath().c_str());
            job_create_status = error(job_ptr, kRsyncBadRemoteJob);
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
   RsyncError job_create_status = kRsyncSuccess;

   JobDescriptor& descriptor = job_ptr->descriptor();

   {
      boost::mutex::scoped_lock guard( callback_lock_ );

      job_create_status = job_request_callback_ptr_->canFulfill(
         job_ptr->fileSystem(),
         job_ptr->descriptor()
      );
   }

   if ( job_create_status != kRsyncSuccess )
   {
      job_create_status = error( job_ptr, job_create_status );
   }

   // If the source or destination is remote, check that the RsyncPacketRouter
   // has been registered with a packet router and that the JobAgent has been
   // registered with the RsyncPacketRouter.
   if ( job_create_status == kRsyncSuccess )
   {
      if ( descriptor.getSource().remote() ||
           descriptor.getDestination().remote() )
      {
         if ( ( packet_router_.isSubscribed() == false ) ||
              ( this->isSubscribed() == false ) )
         {
            log::error(
               "JobAgent::createJob - "
               "Packet subscription not initialized. "
               "Can't complete remote job.\n");

            job_create_status = error( job_ptr, kRsyncNotSubscribed );
         }
      }
   }

   // If the destination is local, verify that it exists. If it does not, but
   // stub creation is enabled, attempt to create it with touch. If stub
   // creation is not enabled, job creation failed.
   if ( job_create_status == kRsyncSuccess )
   {
      if ( descriptor.getDestination().remote() == false )
      {
         boost::mutex::scoped_lock guard( callback_lock_ );

         // If the destination file does not exist, create it (if so configured).
         if ( job_request_callback_ptr_->canTouchDestination() )
         {
            if ( file_sys_interface_.touch( descriptor.getDestinationPath() ) == false )
            {
               log::error(
                  "JobAgent::createJob - "
                  "Failed to touch destination resource\n" );
               job_create_status = error(
                  job_ptr, kRsyncDestinationFileNotFound );
            }
         }
      }
   }


   if ( job_create_status == kRsyncSuccess )
   {
      if ( ( job_create_status = addActiveJob( job_ptr ) ) == kRsyncSuccess )
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
   RsyncError job_create_status = kRsyncSuccess;

   if ( addJobToTable( job_ptr ) )
   {
      if ( job_ptr->descriptor().getDestination().remote() == false )
      {
         job_create_status = worker_group_.addJob( this, job_ptr );
      }

      if ( job_create_status != kRsyncSuccess )
      {
         active_jobs_.erase( job_ptr->descriptor().uuid() );
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
   boost::mutex::scoped_lock guard( active_jobs_lock_ );

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
   RsyncError send_status = kRsyncSuccess;

   // Attempt to send the request. Failure means that the packet was not
   // added to the send queue, so delete it.
   std::string packet_data = descriptor.serialize();

   bool send_success = sendTo(
      RsyncPacket::RsyncJobAgent,
      RsyncPacket::RsyncJobRequest,
      packet_data.data(),
      packet_data.size()
   );

   if ( send_success == false )
   {
      log::error("JobAgent::sendRemoteJob: Failed to send remote job request\n");
      send_status = kRsyncCommError;
   }

   return send_status;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::error( RsyncJob* job_ptr, RsyncError error )
{
   job_ptr->report().createJobStatus = error;

   // 
   if ( ( error != kRsyncSuccess ) && job_ptr->descriptor().isRemoteRequest() )
   {
      RemoteJobResult result( job_ptr->descriptor().uuid(), job_ptr->report() );

      log::status("JobAgent::error: SENDING RSYNC_JOB_COMPLETE - %s\n",
         job_ptr->descriptor().isRemoteRequest() ? "REMOTE" : "LOCAL" );
      std::string packet_data = result.serialize();
      bool send_success = sendTo(
         RsyncPacket::RsyncJobAgent,
         RsyncPacket::RsyncJobComplete,
         packet_data.data(),
         packet_data.size() );

      if ( send_success == false )
      {
         log::error("JobAgent::error: Failed to transmit error to remote node\n");
         error = kRsyncCommError;
      }
   }

   return error;
}

//----------------------------------------------------------------------------
void JobAgent::releaseJobIfReleasable( RsyncJob* job_ptr )
{
   if ( job_ptr->done() || job_ptr->descriptor().isRemoteRequest() )
   {
      releaseJob( job_ptr );
   }
}

//----------------------------------------------------------------------------
void JobAgent::releaseJob( RsyncJob* job_ptr )
{
   JobDescriptor& descriptor = job_ptr->descriptor();

   log::debug(
      "JobAgent::releaseJob: Releasing %s job\n",
      descriptor.isRemoteRequest() ? "REMOTE" : "LOCAL" );

   // Notify the remote node that a job has been completed.
   if ( descriptor.isRemoteRequest() &&
      ( descriptor.getDestination().remote() == false ) )
   {
      RemoteJobResult result( descriptor.uuid(), job_ptr->report() );

      std::string packet_data = result.serialize();
      bool send_success = sendTo(
         RsyncPacket::RsyncJobAgent,
         RsyncPacket::RsyncJobComplete,
         packet_data.data(),
         packet_data.size() );

      if ( send_success == false )
      {
         log::error("JobAgent::releaseJob: Failed to release job\n");
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
bool JobAgent::processPacket( const void* data_ptr, ui32 length )
{
   bool route_success = false;
   RsyncPacketLite packet( data_ptr, length );


   if ( packet.valid() )
   {
      route_success = true;

      switch ( packet.header()->type )
      {
         case RsyncPacket::RsyncJobRequest:
            createJob( (const char*)packet.data(), packet.header()->length );
            break;

         case RsyncPacket::RsyncJobComplete:
            finishRemoteJob(
               (const char*)packet.data(),
               packet.header()->length );
            break;

         case RsyncPacket::RsyncRemoteAuthQuery:
            processRemoteAuthRequest( packet.data(), packet.header()->length );
            break;

         default:
            // log::error("JobAgent - Unrecognized packet type");
            route_success = false;
            break;
      }
   }

   return route_success;
}

//----------------------------------------------------------------------------
void JobAgent::processRemoteAuthRequest( const void* data_ptr, ui32 length )
{
   JobDescriptor descriptor( (const char*)data_ptr, length );

   if ( descriptor.isValid() )
   {
      RsyncJob* job_ptr = NULL;
      bool new_job      = false;

      {
         boost::mutex::scoped_lock guard( active_jobs_lock_ );

         JobTable::iterator job_iterator =
            active_jobs_.find( descriptor.uuid() );

         if ( job_iterator != active_jobs_.end() )
         {
            job_ptr = job_iterator->second;
         }
         else
         {
            // Indicate that the job was remotely requested.
            descriptor.setRemoteRequest();

            // This is an auth request, so by definition the destination must
            // be remote.
            descriptor.getDestination().setRemote( true );

            // If a remote JobAgent specifies a remote source, the source is
            // now local (and vice versa).
            descriptor.getSource().setRemote( false );

            job_ptr = new (std::nothrow) RsyncJob(
               file_sys_interface_,
               packet_router_
            );

            job_ptr->setDescriptor( descriptor );

            new_job = true;
         }
      }

      RsyncError status = kRsyncSuccess;
      {
         boost::mutex::scoped_lock guard( callback_lock_ );

         status = job_request_callback_ptr_->canFulfill(
            job_ptr->fileSystem(),
            job_ptr->descriptor()
         );
      }

      if ( status == kRsyncSuccess )
      {
         bool can_execute_job = true;

         // If the job is new, add it to the active job table.
         if ( new_job )
         {
            if ( addJobToTable( job_ptr ) == false )
            {
               can_execute_job = false;
            }
         }

         if ( can_execute_job )
         {
            status = worker_group_.addJob( this, job_ptr );
         }
         else
         {
            log::error("JobAgent::handleRemoteJobRequest: FAILED TO ADD JOB\n");
            status = kRsyncJobTableInsertError;
         }
      }

      if ( status != kRsyncSuccess )
      {
         setActiveJob( job_ptr );

         RsyncQueryResponse response( descriptor.uuid(), status );

         std::string packet_data = response.serialize();

         sendTo(
            RsyncPacket::RsyncAuthorityInterface,
            RsyncPacket::RsyncRemoteAuthAcknowledgment,
            packet_data.data(),
            packet_data.size() );

         unsetActiveJob();

         delete job_ptr;
      }
   }
   else
   {
      log::error("RemoteAuthorityService::handleRemoteJobRequest - "
                    "Failed to deserialize JobDescriptor\n");
   }
}

//----------------------------------------------------------------------------
bool JobAgent::addJobToTable( RsyncJob* job_ptr )
{
   bool add_success = false;

   if ( allow_new_jobs_  )
   {
      boost::mutex::scoped_lock guard( active_jobs_lock_ );

      std::pair<JobTable::iterator,bool> insert_status;
      std::pair<buuid, RsyncJob*> job_pair;

      job_pair = std::make_pair( job_ptr->descriptor().uuid(), job_ptr );
      insert_status = active_jobs_.insert( job_pair );

      add_success = insert_status.second;
   }

   return add_success;
}

//----------------------------------------------------------------------------
void JobAgent::finishRemoteJob( const char* data_ptr, ui32 size_bytes )
{
   RemoteJobResult job_result;

   if ( job_result.deserialize( data_ptr,  size_bytes ) )
   {
      RsyncJob* job_ptr = NULL;

      {
         boost::mutex::scoped_lock guard( active_jobs_lock_ );

         JobTable::iterator job_iterator =
            active_jobs_.find( job_result.uuid() );

         if ( job_iterator != active_jobs_.end() )
         {
            job_ptr = job_iterator->second;
         }
      }

      if ( job_ptr )
      {
         job_ptr->mergeReport( job_result.report() );

         if ( job_ptr->descriptor().getDestination().remote() )
         {
            if ( job_ptr->report().createJobStatus != kRsyncSuccess )
            {
               log::error("JobAgent::finishRemoteJob: Failed to create job - %s\n",
                  errorToString( job_ptr->report().createJobStatus ).c_str() );
            }

            releaseJobIfReleasable( job_ptr );
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


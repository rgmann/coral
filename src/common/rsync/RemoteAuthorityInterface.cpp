#include <boost/current_function.hpp>
#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "RsyncPacketRouter.h"
#include "RemoteAuthorityInterface.h"

#define  DEFAULT_SEGMENT_TIMEOUT_MS     (10000)

using namespace liber;
using namespace liber::rsync;
using namespace liber::thread;


RemoteAuthorityInterface::ActiveJob::ActiveJob()
: job_ptr_(NULL)
, query_response_(RsyncSuccess)
{
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::ActiveJob::setJob( RsyncJob* job_ptr )
{
   job_ptr_ = job_ptr;
   mLastInstructionTime = boost::posix_time::microsec_clock::local_time();
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::ActiveJob::pushInstruction( Instruction* pInstruction )
{
  if ( pInstruction && job_ptr_ )
  {
    job_ptr_->instructions().push( pInstruction );

    // Update last receive timestamp.
    mLastInstructionTime = boost::posix_time::microsec_clock::local_time();
  }
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::ActiveJob::setQueryResponse(
   const void* data_ptr, ui32 length )
{
   RsyncQueryResponse response;

   if ( response.deserialize( (const char*)data_ptr, length ) )
   {
      if ( response.uuid() == job_ptr_->descriptor().uuid() )
      {
         query_response_ = response.status();
      }
      else
      {
         query_response_ = kRsyncRemoteQueryError;
         liber::log::error("RemoteAuthorityInterface::ActiveJob::setQueryResponse: Invalid UUID\n");
      }
   }
   else
   {
      query_response_ = kRsyncRemoteQueryError;
      liber::log::error("RemoteAuthorityInterface::ActiveJob::setQueryResponse: Failed to deserialize\n");
   }

   job_start_signal_.give();
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::ActiveJob::signalJobStart()
{
   job_start_signal_.give();
}

//----------------------------------------------------------------------------
bool RemoteAuthorityInterface::ActiveJob::waitJobStart( int timeout_ms )
{
   return ( job_start_signal_.take( timeout_ms ) == BinarySem::SemAcquired );
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::ActiveJob::signalJobEnd()
{
   job_end_signal_.give();
}

//----------------------------------------------------------------------------
bool RemoteAuthorityInterface::ActiveJob::waitJobEnd( int timeout_ms )
{
   return ( job_end_signal_.take( timeout_ms ) == BinarySem::SemAcquired );
}

//----------------------------------------------------------------------------
bool RemoteAuthorityInterface::ActiveJob::lockIfActive()
{
   job_lock_.lock();
   bool lbActive = (job_ptr_ != NULL);
   if (!lbActive) job_lock_.unlock();
   return lbActive;
}

//----------------------------------------------------------------------------
bool RemoteAuthorityInterface::ActiveJob::timeout()
{
   boost::posix_time::time_duration deltaTime =
     boost::posix_time::microsec_clock::local_time() - mLastInstructionTime;
   return ( deltaTime.total_milliseconds() >= JOB_TIMEOUT_MS );
}

//----------------------------------------------------------------------------
RemoteAuthorityInterface::RemoteAuthorityInterface()
: mnSegmentTimeoutMs(DEFAULT_SEGMENT_TIMEOUT_MS)
{
}

//----------------------------------------------------------------------------
RemoteAuthorityInterface::~RemoteAuthorityInterface()
{
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::process( RsyncJob* job_ptr )
{
   // Subscribe to the jobs packet subscriber.
   if ( job_ptr->packetRouter().addSubscriber(
      RsyncPacket::RsyncAuthorityInterface, this ) == false ) {
      liber::log::error("RemoteAuthorityInterface::process: FAILED to register\n");
   }

   // Send the JobDescriptor and then wait for an acknowledgement from the
   // RsyncService.  The RsyncService may send an acknowledgement saying that
   // it cannot handle the request. In this case, we drain the segments and
   // send an end instruction to the Assembler. A timeout is handled in the
   // same way, but different status is reported.
   RsyncError process_status = startRemoteJob( job_ptr );

   // Read all segments as they become available from the SegmentQueue. If the
   // remote job was not successfully started, this will simply delete the
   // segments. If this remote job was successfully started, flushSegments will
   // send the segments before deleting them.
   if ( flushSegments( job_ptr->segments(), ( process_status == RsyncSuccess ) ) == false )
   {
      process_status = kRsyncCommError;
      liber::log::debug("RemoteAuthorityInterface::process: kRsyncCommError\n");
   }

   // If the remote Authority job was successfully started,
   // wait until End instruction is received from the remote job.
   if ( process_status == RsyncSuccess )
   {
      process_status = waitForEndInstruction(
         job_ptr->descriptor().completionTimeoutMs() );
   }

   // Regardles of success or failure, release the active job. The active job
   // is locked to prevent "put" from accessing the RsyncJob while the job is
   // being released. Note: This resets ActiveJob's shared pointer to the
   // active RsyncJob, but not the local pointer (so cancelAssembly, if
   // executed will not encounter a race condition).
   releaseActiveJob();

   // If the remote job failed to start for some reason, or if an error, such
   // as a timeout, occurred while waiting for the End instruction, create
   // an End instruction that notifies the Assembler of the error.
   if ( process_status != RsyncSuccess )
   {
      cancelAssembly( job_ptr->instructions(), process_status );
   }

   // Unsubscribe from this jobs packet router.
   job_ptr->packetRouter().removeSubscriber(
      RsyncPacket::RsyncAuthorityInterface );
}

//-----------------------------------------------------------------------------
bool RemoteAuthorityInterface::put( const char* data_ptr, ui32 length )
{
   bool put_success = false;
   RsyncPacket* packet_ptr = new RsyncPacket();

   if ( packet_ptr->unpack( data_ptr, length ) )
   {
      // Lock the job state mutex so that the main thread cannot create or
      // delete the RsyncJobState member.
      if ( active_job_.lockIfActive() )
      {
         switch ( packet_ptr->data()->type )
         {
            case RsyncPacket::RsyncRemoteAuthAcknowledgment:
               active_job_.setQueryResponse(
                  packet_ptr->dataPtr(),
                  packet_ptr->data()->length );
               break;

            case RsyncPacket::RsyncInstruction:
               sendAssemblyInstruction(
                  packet_ptr->dataPtr(),
                  packet_ptr->data()->length );
               break;

            case RsyncPacket::RsyncAuthorityReport:
               setSourceReport(
                  packet_ptr->dataPtr(),
                  packet_ptr->data()->length );
               break;

            default: break;
         }

         active_job_.unlock();
      }
      else
      {
         log::error("RemoteAuthorityInterface::put - Failed to lock active job\n");
      }
   }

   return put_success;
}

//-----------------------------------------------------------------------------
RsyncError RemoteAuthorityInterface::waitForEndInstruction(int nTimeoutMs)
{
  RsyncError status = RsyncSuccess;

  while ( true )
  {
    if ( active_job_.waitJobEnd( 100 ) )
    {
      break;
    }
    else if ( active_job_.timeout() )
    {
      status = RsyncRemoteJobTimeout;
      break;
    }
  }

  return status;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::releaseActiveJob()
{
   if (active_job_.lockIfActive())
   {
      active_job_.setJob(NULL);
      active_job_.queryResponse() = RsyncSuccess;
      active_job_.unlock();
   }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::
sendAssemblyInstruction(const void* data_ptr, ui32 byte_count )
{
   Instruction* instruction_ptr = NULL;

   std::string instruction_data;
   instruction_data.assign( (const char*)data_ptr, byte_count );

   instruction_ptr = InstructionFactory::Deserialize( instruction_data );

   if ( instruction_ptr )
   {
      // Send the instruction to the assembler.
      active_job_.pushInstruction( instruction_ptr );
   }
   else
   {
      log::error("Failed to derialize instruction.\n");
   }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::setSourceReport(
   const void* data_ptr,
   ui32        byte_count
)
{
   RsyncJob* job_ptr = job_ptr = active_job_.job();

   if ( job_ptr != NULL )
   {
      SourceReport& source_report = job_ptr->report().source;

      if ( source_report.deserialize( (const char*)data_ptr, byte_count ) == false )
      {
         log::error(
            "%s - Failed to deserialize source report.\n",
            BOOST_CURRENT_FUNCTION );

         source_report.authority.status = kRsyncCommError;
      }

      // If this is the source report, RemoteAuthorityInterface is finished
      // with the active job.  Signal completion to the main thread.
      active_job_.signalJobEnd();
   }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::cancelAssembly(
   InstructionQueue& instructions,
   RsyncError        status)
{
   EndInstruction* end_instr_ptr = new EndInstruction();
   end_instr_ptr->cancel( status );
   instructions.push( end_instr_ptr );
}

//-----------------------------------------------------------------------------
RsyncError RemoteAuthorityInterface::startRemoteJob( RsyncJob* job_ptr )
{
   RsyncError query_status = RsyncSuccess;

   RsyncPacket* packet_ptr = new RsyncPacket(
      RsyncPacket::RsyncRemoteAuthQuery,
      job_ptr->descriptor().serialize()
   );

   active_job_.setJob( job_ptr );

   if ( sendPacketTo( RsyncPacket::RsyncJobAgent, packet_ptr ) )
   {
      if ( active_job_.waitJobStart( job_ptr->descriptor().completionTimeoutMs() ) )
      {
         query_status = active_job_.queryResponse();
         job_ptr->report().source.authority.status = query_status;
      }
      else
      {
         log::error(
            "%s - RSYNC remote query timeout.\n", BOOST_CURRENT_FUNCTION );

         query_status = RsyncRemoteQueryTimeout;
         job_ptr->report().source.authority.status = query_status;
      }
   }
   else
   {
      log::error(
         "%s - Failed to send job query.\n", BOOST_CURRENT_FUNCTION );

      query_status = kRsyncCommError;
      job_ptr->report().source.authority.status = query_status;
   }

   delete packet_ptr;

   return query_status;
}

//-----------------------------------------------------------------------------
bool RemoteAuthorityInterface::flushSegments(
  SegmentQueue& segments,
  bool          send_segments
)
{
   bool flush_success = true;
   bool end_of_stream = false;

   while ( end_of_stream == false )
   {
      Segment* segment_ptr = NULL;

      if ( segments.pop( &segment_ptr, mnSegmentTimeoutMs ) && segment_ptr )
      {
         end_of_stream = segment_ptr->endOfStream();

         if ( send_segments )
         {
            RsyncPacket* packet_ptr = new RsyncPacket(
               RsyncPacket::RsyncSegment,
               segment_ptr->serialize()
            );

            if ( flush_success )
            {
               flush_success = sendPacketTo(
                  RsyncPacket::RsyncAuthorityService,
                  packet_ptr
               );
            }

            delete packet_ptr;
         }

         delete segment_ptr;
      }
      else
      {
         flush_success = false;
         end_of_stream = true;
      }
   }

   return flush_success;
}



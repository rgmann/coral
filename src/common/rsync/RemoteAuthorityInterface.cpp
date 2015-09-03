#include <boost/current_function.hpp>
#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "RsyncPacketRouter.h"
#include "RemoteAuthorityInterface.h"

#define  DEFAULT_SEGMENT_TIMEOUT_MS    ( 10000 )
#define  JOB_TIMEOUT_MS                ( 2000 )

using namespace liber;
using namespace liber::rsync;
using namespace liber::thread;
using namespace liber::netapp;


RemoteAuthorityInterface::ActiveJob::ActiveJob()
:  job_ptr_(NULL)
,  query_response_(kRsyncSuccess)
{
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::ActiveJob::setQueryResponse(
   const boost::uuids::uuid& expected_job_id,
   const void* data_ptr, ui32 length )
{
   RsyncQueryResponse response;

   if ( response.deserialize( (const char*)data_ptr, length ) )
   {
      if ( response.uuid() == expected_job_id )
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
RemoteAuthorityInterface::RemoteAuthorityInterface()
:  RsyncPacketSubscriber( true )
,  mnSegmentTimeoutMs(DEFAULT_SEGMENT_TIMEOUT_MS)
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
   if ( job_ptr->packetRouter().subscribe(
      RsyncPacket::RsyncAuthorityInterface, this,
      liber::netapp::kSubscriberModeReadWrite ) == false ) {
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
   if ( flushSegments( job_ptr->segments(), ( process_status == kRsyncSuccess ) ) == false )
   {
      process_status = kRsyncCommError;
      liber::log::debug("RemoteAuthorityInterface::process: kRsyncCommError\n");
   }

   // If the remote Authority job was successfully started,
   // wait until End instruction is received from the remote job.
   if ( process_status == kRsyncSuccess )
   {
      process_status = waitForEndInstruction(
         job_ptr->descriptor().completionTimeoutMs() );
   }

   // Regardles of success or failure, release the active job. The active job
   // is locked to prevent "put" from accessing the RsyncJob while the job is
   // being released. Note: This resets ActiveJob's shared pointer to the
   // active RsyncJob, but not the local pointer (so cancelAssembly, if
   // executed will not encounter a race condition).
   // releaseActiveJob();
   unsetActiveJob();

   // If the remote job failed to start for some reason, or if an error, such
   // as a timeout, occurred while waiting for the End instruction, create
   // an End instruction that notifies the Assembler of the error.
   if ( process_status != kRsyncSuccess )
   {
      cancelAssembly( job_ptr->instructions(), process_status );
   }

   // Unsubscribe from this jobs packet router.
   job_ptr->packetRouter().unsubscribe(
      RsyncPacket::RsyncAuthorityInterface, this );
}

//-----------------------------------------------------------------------------
bool RemoteAuthorityInterface::processPacket( const void* data_ptr, ui32 length )
{
   bool put_success = false;
   RsyncPacketLite packet( data_ptr, length );

   if ( packet.valid() )
   {
      switch ( packet.header()->type )
      {
         case RsyncPacket::RsyncRemoteAuthAcknowledgment:
            handleQueryResponse( packet );
            break;

         case RsyncPacket::RsyncInstruction:
            sendAssemblyInstruction(
               packet.data(),
               packet.header()->length );
            break;

         case RsyncPacket::RsyncAuthorityReport:
            setSourceReport(
               packet.data(),
               packet.header()->length );
            break;

         default: break;
      }
   }

   return put_success;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::handleQueryResponse(
   const RsyncPacketLite& packet )
{
   boost::mutex::scoped_lock guard( activeJobLock() );

   RsyncJob* active_job_ptr = activeJob();

   if ( active_job_ptr )
   {
      active_job_.setQueryResponse(
         active_job_ptr->descriptor().uuid(),
         packet.data(),
         packet.header()->length );
   }
   else
   {
      log::error("RemoteAuthorityInterface::handleQueryResponse: NULL active job\n");
   }
}

//-----------------------------------------------------------------------------
RsyncError RemoteAuthorityInterface::waitForEndInstruction(int nTimeoutMs)
{
   RsyncError status = kRsyncSuccess;

   while ( true )
   {
      if ( active_job_.waitJobEnd( 100 ) )
      {
         break;
      }
      else
      {
         // Check whether an instruction has been received recently.
         boost::posix_time::time_duration deltaTime =
            boost::posix_time::microsec_clock::local_time() - last_instruction_time_;
         if ( deltaTime.total_milliseconds() >= JOB_TIMEOUT_MS )
         {
            status = RsyncRemoteJobTimeout;
            break;
         }
      }
   }

   return status;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::sendAssemblyInstruction(
   const void* data_ptr,
   ui32        byte_count )
{
   // InstructionContainer* container_ptr = new InstructionContainer();
   InstructionRaw* instruction_ptr = new InstructionRaw( data_ptr, byte_count );

   if ( instruction_ptr )
   {
      // container_ptr->stream().assign( data_ptr, byte_count );

      boost::mutex::scoped_lock guard( activeJobLock() );

      // Send the instruction to the assembler.
      if ( activeJob() )
      {
         activeJob()->instructions().push( instruction_ptr );

         // Update last receive timestamp.
         last_instruction_time_ = boost::posix_time::microsec_clock::local_time();
      }
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
   boost::mutex::scoped_lock guard( activeJobLock() );

   RsyncJob* job_ptr = activeJob();

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
   EndInstruction instruction;
   instruction.cancel( status );

    // InstructionContainer* container_ptr = new InstructionContainer( instruction.type() );

   // container_ptr->serialize( container_ptr->stream() );
   // instruction.serialize( container_ptr->stream() );

   // instructions.push( container_ptr );
   instructions.push( instruction.instruction() );
}

//-----------------------------------------------------------------------------
RsyncError RemoteAuthorityInterface::startRemoteJob( RsyncJob* job_ptr )
{
   RsyncError query_status = kRsyncSuccess;

   setActiveJob( job_ptr );

   std::string packet_data = job_ptr->descriptor().serialize();
   bool send_success = sendTo(
      RsyncPacket::RsyncJobAgent,
      RsyncPacket::RsyncRemoteAuthQuery,
      packet_data.data(),
      packet_data.size() );

   if ( send_success )
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
            if ( flush_success )
            {
               std::string packet_data = segment_ptr->serialize();

               flush_success = sendTo(
                  RsyncPacket::RsyncAuthorityService,
                  RsyncPacket::RsyncSegment,
                  packet_data.data(),
                  packet_data.size() );
            }
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



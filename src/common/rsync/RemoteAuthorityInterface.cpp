#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "RsyncPacketRouter.h"
#include "RemoteAuthorityInterface.h"

#define  DEFAULT_SEGMENT_TIMEOUT_MS     (10000)
#define  DEFAULT_JOB_TIMEOUT_MS         DEFAULT_SEGMENT_TIMEOUT_MS

using namespace liber;
using namespace liber::rsync;


void RemoteAuthorityInterface::ActiveJob::pushInstruction( Instruction* pInstruction )
{
  if ( pInstruction && mpJob )
  {
    mpJob->instructions().push( pInstruction );

    // Update last receive timestamp.
    mLastInstructionTime = boost::posix_time::microsec_clock::local_time();
  }
}

//----------------------------------------------------------------------------
RemoteAuthorityInterface::RemoteAuthorityInterface()
: mnSegmentTimeoutMs(DEFAULT_SEGMENT_TIMEOUT_MS)
, mnJobAckTimeoutMs(DEFAULT_JOB_TIMEOUT_MS)
, mnJobCompletionTimeoutMs(DEFAULT_JOB_TIMEOUT_MS)
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

  // liber::log::debug("RemoteAuthorityInterface::process: START %d\n", RsyncPacket::RsyncAuthorityInterface);

  // Send the JobDescriptor and then wait for an acknowledgement from the
  // RsyncService.  The RsyncService may send an acknowledgement saying that
  // it cannot handle the request. In this case, we drain the segments and
  // send an end instruction to the Assembler. A timeout is handled in the
  // same way, but different status is reported.
  RsyncError process_status = startRemoteJob( job_ptr );
  liber::log::debug("RemoteAuthorityInterface::process: START REMOTE JOB\n");

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
    process_status = waitForEndInstruction( mnJobCompletionTimeoutMs );
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

  // liber::log::debug("RemoteAuthorityInterface::process: END\n");

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
    if ( mActiveJob.lockIfActive() )
    {
      switch ( packet_ptr->data()->type )
      {
        case RsyncPacket::RsyncRemoteAuthAcknowledgment:
          mActiveJob.setQueryResponse(
            packet_ptr->dataPtr(), packet_ptr->data()->length );
          break;

        case RsyncPacket::RsyncInstruction:
          sendAssemblyInstruction(
            packet_ptr->dataPtr(), packet_ptr->data()->length );
          break;

        case RsyncPacket::RsyncAuthorityReport:
          setSourceReport( packet_ptr->dataPtr(), packet_ptr->data()->length );
          break;

        default: break;
      }

      mActiveJob.unlock();
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
    if ( mActiveJob.waitJobEnd( 100 ) )
    {
      break;
    }
    else if ( mActiveJob.timeout() )
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
  if (mActiveJob.lockIfActive())
  {
    mActiveJob.setJob(NULL);
    mActiveJob.queryResponse() = RsyncSuccess;
    mActiveJob.unlock();
  }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::
sendAssemblyInstruction(const void* pData, ui32 nBytes)
{
  Instruction* lpInstruction = NULL;

  std::string lInstructionData;
  lInstructionData.assign((const char*)pData, nBytes);

  lpInstruction = InstructionFactory::Deserialize(lInstructionData);

  if (lpInstruction)
  {
    // Send the instruction to the assembler.
    mActiveJob.pushInstruction( lpInstruction );
  }
  else
  {
    log::error("Failed to derialize instruction.\n");
  }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::setSourceReport(const void* pData, ui32 nBytes)
{
  mActiveJob.job()->report().source.deserialize((const char*)pData, nBytes);
  liber::log::debug("RemoteAuthorityInterface::setSourceReport:\n");

  // If this is the source report, RemoteAuthorityInterface is finished
  // with the active job.  Signal completion to the main thread.
  mActiveJob.signalJobEnd();
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::
cancelAssembly(InstructionQueue& instructions, RsyncError status)
{
  EndInstruction* lpEndInstruction = new EndInstruction();
  lpEndInstruction->cancel(status);
  instructions.push(lpEndInstruction);
}

//-----------------------------------------------------------------------------
RsyncError RemoteAuthorityInterface::startRemoteJob( RsyncJob* job_ptr )
{
  RsyncError query_status = RsyncSuccess;

  RsyncPacket* lpPacket = new RsyncPacket(
    RsyncPacket::RsyncRemoteAuthQuery,
    job_ptr->descriptor().serialize()
  );

  mActiveJob.setJob( job_ptr );

  liber::log::debug("RemoteAuthorityInterface::startRemoteJob: SENDING\n");
  if ( sendPacketTo( RsyncPacket::RsyncJobAgent, lpPacket ) )
  {
    liber::log::debug("RemoteAuthorityInterface::startRemoteJob: WAITING\n");
    if (mActiveJob.waitJobStart(mnJobAckTimeoutMs))
    {
      query_status = mActiveJob.queryResponse();
    }
    else
    {
      log::error("RemoteAuthorityInterface - RSYNC remote query timeout.\n");
      query_status = RsyncRemoteQueryTimeout;
    }
  }
  else
  {
    log::error("RemoteAuthorityInterface - Failed to send job query.\n");
  }

  delete lpPacket;

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
          flush_success = sendPacketTo( RsyncPacket::RsyncAuthorityService, packet_ptr );
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



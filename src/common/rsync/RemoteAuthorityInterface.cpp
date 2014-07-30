#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "RemoteAuthorityInterface.h"

#define  DEFAULT_SEGMENT_TIMEOUT_MS     (10000)
#define  DEFAULT_JOB_TIMEOUT_MS         DEFAULT_SEGMENT_TIMEOUT_MS

using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
RemoteAuthorityInterface::RemoteAuthorityInterface()
: mnSegmentTimeoutMs(DEFAULT_SEGMENT_TIMEOUT_MS)
, mnJobAckTimeoutMs(DEFAULT_JOB_TIMEOUT_MS)
, mnJobCompletionTimeoutMs(DEFAULT_JOB_TIMEOUT_MS)
, mRequestID(RsyncPacket::RsyncAuthorityService)
{
}

//----------------------------------------------------------------------------
RemoteAuthorityInterface::~RemoteAuthorityInterface()
{
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::setRequestID(int requestID)
{
  mRequestID = requestID;
}

//----------------------------------------------------------------------------
void RemoteAuthorityInterface::process(RsyncJob* pJob)
{
  // Send the JobDescriptor and then wait for an acknowledgement from the
  // RsyncService.  The RsyncService may send an acknowledgement saying that
  // it cannot handle the request. In this case, we drain the segments and
  // send an end instruction to the Assembler. A timeout is handled in the
  // same way, but different status is reported.
  RsyncError lStatus = startRemoteJob(pJob);

  // Read all segments as they become available from the SegmentQueue. If the
  // remote job was not successfully started, this will simply delete the
  // segments. If this remote job was successfully started, flushSegments will
  // send the segments before deleting them.
  flushSegments(pJob->segments(), (lStatus == RsyncSuccess));

  // If the remote Authority job was successfully started,
  // wait until End instruction is received from the remote job.
  if (lStatus == RsyncSuccess)
  {
    lStatus = waitForEndInstruction(mnJobCompletionTimeoutMs);
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
  if (lStatus != RsyncSuccess)
  {
    cancelAssembly(pJob->instructions(), lStatus);
  }
}

//-----------------------------------------------------------------------------
bool RemoteAuthorityInterface::put(const char* pData, ui32 nLength)
{
  bool lbSuccess = false;
  RsyncPacket* lpPacket = new RsyncPacket();

  if (lpPacket->unpack(pData, nLength))
  {
    // Lock the job state mutex so that the main thread cannot create or
    // delete the RsyncJobState member.
    if (mActiveJob.lockIfActive())
    {
      switch (lpPacket->data()->type)
      {
        case RsyncPacket::RsyncRemoteAuthAcknowledgment:
          memcpy(&mActiveJob.queryResponse(),
                 lpPacket->dataPtr(),
                 lpPacket->data()->length);

          mActiveJob.signalJobStart();
          break;

        case RsyncPacket::RsyncInstruction:
          sendAssemblyInstruction(lpPacket->dataPtr(),
                                  lpPacket->data()->length);
          break;

        case RsyncPacket::RsyncAuthorityReport:
          if (mActiveJob.lockIfActive())
          {
            mActiveJob.job()->report().source.authority.deserialize(pData, nLength);
            mActiveJob.unlock();
          }
          break;

        default: break;
      }

      mActiveJob.unlock();
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
RsyncError RemoteAuthorityInterface::waitForEndInstruction(int nTimeoutMs)
{
  return mActiveJob.waitJobEnd(nTimeoutMs) ? RsyncSuccess : RsyncRemoteJobTimeout;
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
sendAssemblyInstruction(const void* pData, ui32 nLength)
{
  Instruction* lpInstruction = NULL;

  std::string lInstructionData;
  lInstructionData.assign((const char*)pData, nLength);

  lpInstruction = InstructionFactory::Deserialize(lInstructionData);

  if (lpInstruction)
  {
    // Spy on the instruction type before sending the instruction.
    ui32 lInstructionType = lpInstruction->type();

    // Send the instruction to the assembler.
    mActiveJob.job()->instructions().push(lpInstruction);

    // If this is and end instruction, RemoteAuthorityInterface is finished
    // with the active job.  Signal completion to the main thread.
    if (lInstructionType == EndInstruction::Type)
    {
      mActiveJob.signalJobEnd();
    }
  }
  else
  {
    log::error("Failed to derialize instruction.\n");
  }
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
RsyncError RemoteAuthorityInterface::
startRemoteJob(RsyncJob* pJob)
{
  RsyncError lQueryStatus = RsyncSuccess;

  RsyncPacket* lpPacket = new RsyncPacket(RsyncPacket::RsyncRemoteAuthQuery,
                                          pJob->descriptor().serialize());
  mActiveJob.setJob(pJob);

  if (sendPacketTo(mRequestID, lpPacket))
  {
    if (mActiveJob.waitJobStart(mnJobAckTimeoutMs))
    {
      lQueryStatus = mActiveJob.queryResponse();
    }
    else
    {
      log::error("RemoteAuthorityInterface - RSYNC remote query timeout.\n");
      lQueryStatus = RsyncRemoteQueryTimeout;
    }
  }
  else
  {
    log::error("RemoteAuthorityInterface - Failed to send job query.\n");
  }

  delete lpPacket;

  return lQueryStatus;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityInterface::
flushSegments(SegmentQueue& rSegments, bool bSend)
{
  bool lbEndOfStream = false;
  while (lbEndOfStream == false)
  {
    Segment* lpSegment = NULL;

    if (rSegments.pop(&lpSegment, mnSegmentTimeoutMs) && lpSegment)
    {
      lbEndOfStream = lpSegment->endOfStream();

      if (bSend)
      {
        RsyncPacket* lpPacket = new RsyncPacket(RsyncPacket::RsyncSegment,
                                                lpSegment->serialize());

        sendPacketTo(mRequestID, lpPacket);
        delete lpPacket;
      }

      delete lpSegment;
      lpSegment = NULL;
    }
    else
    {
      lbEndOfStream = true;
    }
  }
}



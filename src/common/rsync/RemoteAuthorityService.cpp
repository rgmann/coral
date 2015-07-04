#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "Segment.h"
#include "RemoteAuthorityService.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
JobQueue::
JobQueue(FileSystemInterface& rFileSys,
         InstructionReceiver& rReceiver,
         JobCompletionHook& rJobHook)
: IThread("JobQueue")
, mAuthority(rFileSys)
, mrReceiver(rReceiver)
, mrJobHook(rJobHook)
, mpActiveJob(NULL)
{
  mJobQueue.initialize();
}

//-----------------------------------------------------------------------------
RsyncJob* JobQueue::activeJob()
{
  return mpActiveJob;
}

//-----------------------------------------------------------------------------
Mutex& JobQueue::lock()
{
  return mJobLock;
}

//-----------------------------------------------------------------------------
bool JobQueue::lockIfActive()
{
  mJobLock.lock();
  bool lbIsActive = (mpActiveJob != NULL);
  if (!lbIsActive) mJobLock.unlock();
  return lbIsActive;
}

//-----------------------------------------------------------------------------
bool JobQueue::push(RsyncJob* pJob)
{
  return mJobQueue.push(pJob);
}

//-----------------------------------------------------------------------------
void JobQueue::run(const bool& bShutdown)
{
  while ( !bShutdown )
  {
    RsyncJob* lpJob = NULL;

    if ( mJobQueue.pop(lpJob) && lpJob )
    {
      mJobLock.lock();
      mpActiveJob = lpJob;
      mJobLock.unlock();

      mAuthority.process(lpJob, mrReceiver);      

      mJobLock.lock();
      mpActiveJob = NULL;
      mJobLock.unlock();

      mrJobHook(lpJob);

      // We're done with the job
      delete lpJob;
    }
  }
}

//-----------------------------------------------------------------------------
RemoteAuthorityService::
  RemoteAuthorityService(FileSystemInterface& rFileSys)
: PacketSubscriber()
, mrFileSys(rFileSys)
, mpUserHandler(NULL)
, mRequestID(RsyncPacket::RsyncAuthorityInterface)
, mSendReportHook(*this)
, mJobQueue(rFileSys, *this, mSendReportHook)
{
  mJobQueue.launch();
  mSendReportHook.setRequestID(mRequestID);
}

//-----------------------------------------------------------------------------
RemoteAuthorityService::~RemoteAuthorityService()
{
  mJobQueue.cancel(true);
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::setRequestID(int requestID)
{
  mRequestID = requestID;
  mSendReportHook.setRequestID(mRequestID);
}

//-----------------------------------------------------------------------------
bool RemoteAuthorityService::put(const char* pData, ui32 nLength)
{
  bool lbSuccess = false;
  RsyncPacket* lpPacket = new RsyncPacket();

  if (lpPacket->unpack(pData, nLength))
  {
    switch (lpPacket->data()->type)
    {
      case RsyncPacket::RsyncRemoteAuthQuery:
        handleRemoteJobRequest(lpPacket->dataPtr(), lpPacket->data()->length);
        lbSuccess = true;
        break;

      case RsyncPacket::RsyncSegment:
        pushSegment(lpPacket->dataPtr(), lpPacket->data()->length);
        lbSuccess = true;
        break;

      default:
        log::debug("RemoteAuthorityService::put - invalid packet type=%d\n",
                   lpPacket->data()->type);
        break;
    }
  }
  else
  {
    log::debug("RemoteAuthorityService::put - failed to unpack packet\n");
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::call(Instruction* pInstruction)
{
  if (pInstruction)
  {
    sendPacketTo(mRequestID,
                 new RsyncPacket(RsyncPacket::RsyncInstruction,
                 InstructionFactory::Serialize(pInstruction)));

    delete pInstruction;
  }
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::
handleRemoteJobRequest(const void* pData, ui32 nLength)
{
  RsyncJob* lpJob = NULL;

  if ((lpJob = new (std::nothrow) RsyncJob()))
  {
    JobDescriptor& rDescriptor = lpJob->descriptor();

    if (rDescriptor.deserialize((const char*)pData, nLength))
    {
      RsyncError lStatus = RsyncSuccess;

      if (mpUserHandler)
      {
        lStatus = mpUserHandler(rDescriptor);
      }
      else
      {
        lStatus = defaultQueryHandler(rDescriptor);
        log::status("REMOTE_AUTH_SERVICE: %s, %d\n", rDescriptor.getSourcePath().c_str(), lStatus);
      }

      if (lStatus == RsyncSuccess)
      {
        mJobQueue.push(lpJob);
      }

      sendPacketTo(mRequestID, 
                   new (std::nothrow) RsyncPacket(
                   RsyncPacket::RsyncRemoteAuthAcknowledgment,
                   sizeof(lStatus), &lStatus));
    }
    else
    {
      log::error("RemoteAuthorityService::handleRemoteJobRequest - "
                 "Failed to deserialize JobDescriptor\n");
      delete lpJob;
    }
  }
  else
  {
    log::error("RemoteAuthorityService::handleRemoteJobRequest - "
               "Failed to allocate RsyncJob\n");
  }
}

//-----------------------------------------------------------------------------
RsyncError RemoteAuthorityService::
defaultQueryHandler(JobDescriptor& rJob)
{
  RsyncError lStatus = RsyncSuccess;

  if (mrFileSys.exists(rJob.getSource().path) == false)
  {
    lStatus = RsyncSourceFileNotFound;
    log::error("Remote job query failed for %s\n",
               rJob.getSource().path.string().c_str());
  }

  return lStatus;
}

//-----------------------------------------------------------------------------
void RemoteAuthorityService::pushSegment(const void* pData, ui32 nLength)
{
  if (mJobQueue.lockIfActive())
  {
    Segment* lpSegment = new Segment();
    if (lpSegment->deserialize((const char*)pData, nLength))
    {
      mJobQueue.activeJob()->segments().push(lpSegment);
    }
    else
    {
      log::error("Failed to deserialize segment.\n");
      delete lpSegment;
    }

    mJobQueue.lock().unlock();
  }
  else
  {
    log::error("No job active to receive segment.\n");
  }
}

//----------------------------------------------------------------------------
RemoteAuthorityService::SendReportHook::
SendReportHook(PacketSubscriber& rSubscriber)
: mrSubscriber(rSubscriber)
{
}

//----------------------------------------------------------------------------
void RemoteAuthorityService::SendReportHook::setRequestID(int requestID)
{
  mRequestID = requestID;
}

//----------------------------------------------------------------------------
void RemoteAuthorityService::SendReportHook::operator () (RsyncJob* pJob)
{
  if (pJob)
  {
    JobReport& rReport = pJob->report();

    bool lbSuccess = mrSubscriber.sendPacketTo(mRequestID, new RsyncPacket(
                                  RsyncPacket::RsyncAuthorityReport,
                                  rReport.source.serialize()));
    if (lbSuccess == false)
    {
      log::error("SendReportHook - Failed to send report to %d\n", mRequestID);
    }
  }
  else
  {
    log::error("SendReportHook - Received NULL job\n");
  }
}


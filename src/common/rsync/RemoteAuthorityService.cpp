#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "Segment.h"
#include "RemoteAuthorityService.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
JobQueue::JobQueue(
  FileSystemInterface& rFileSys,
  InstructionReceiver& rReceiver,
  JobCompletionHook& rJobHook)
: IThread("JobQueue")
, mAuthority(rFileSys)
, mrReceiver(rReceiver)
, mrJobHook(rJobHook)
, mpActiveJob(NULL)
{
}

//-----------------------------------------------------------------------------
RsyncJob* JobQueue::activeJob()
{
  return mpActiveJob;
}

//-----------------------------------------------------------------------------
boost::mutex& JobQueue::lock()
{
  return job_lock_;
}

//-----------------------------------------------------------------------------
bool JobQueue::lockIfActive()
{
  job_lock_.lock();
  bool lbIsActive = (mpActiveJob != NULL);
  if (!lbIsActive) job_lock_.unlock();
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
      job_lock_.lock();
      mpActiveJob = lpJob;
      job_lock_.unlock();

      mAuthority.process( lpJob, mrReceiver );      

      job_lock_.lock();
      mpActiveJob = NULL;
      job_lock_.unlock();

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
  RsyncJob* job_ptr = NULL;

  if ( ( job_ptr = new (std::nothrow) RsyncJob() ) )
  {
    JobDescriptor& descriptor = job_ptr->descriptor();

    if (descriptor.deserialize((const char*)pData, nLength))
    {
      RsyncError status = RsyncSuccess;

      if (mpUserHandler)
      {
        status = mpUserHandler(descriptor);
      }
      else
      {
        status = defaultQueryHandler(descriptor);
      }

      //
      // If a job was successfully created, add it to the job queue.
      if (status == RsyncSuccess)
      {
        mJobQueue.push(job_ptr);
      }

      sendPacketTo(mRequestID, 
                   new (std::nothrow) RsyncPacket(
                   RsyncPacket::RsyncRemoteAuthAcknowledgment,
                   sizeof(status), &status));
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

    bool lbSuccess = mrSubscriber.sendPacketTo(
      mRequestID,
      new RsyncPacket(
        RsyncPacket::RsyncAuthorityReport,
        rReport.source.serialize()
    ));

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


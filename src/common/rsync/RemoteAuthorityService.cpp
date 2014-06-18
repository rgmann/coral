#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "Segment.h"
#include "RemoteAuthorityService.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
JobQueue::JobQueue(FileSystemInterface& rFileSys, InstructionReceiver& rReceiver)
: IThread("JobQueue")
, mAuthority(rFileSys)
, mrReceiver(rReceiver)
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
  while (!bShutdown)
  {
    RsyncJob* lpJob = NULL;
    if (mJobQueue.pop(lpJob) && lpJob)
    {
      mJobLock.lock();
      mpActiveJob = lpJob;
      mJobLock.unlock();

      mAuthority.process(lpJob, mrReceiver);

      mJobLock.lock();
      mpActiveJob = NULL;
      mJobLock.unlock();
    }
  }
}

//-----------------------------------------------------------------------------
RemoteAuthorityService::
  RemoteAuthorityService(FileSystemInterface& rFileSys)
: PacketSubscriber()
, mrFileSys(rFileSys)
, mpUserHandler(NULL)
, mRequestID(-1)
, mJobQueue(rFileSys, *this)
{
  mJobQueue.launch();
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
}

//-----------------------------------------------------------------------------
bool RemoteAuthorityService::put(const char* pData, ui32 nLength)
{
  bool lbSuccess = false;
  RsyncPacket* lpPacket = new RsyncPacket();

  if (lpPacket->unpack(pData, nLength))
  {
    log::status("RemoteAuthorityService::put - Received packet length=%u\n", lpPacket->allocatedSize());
    lpPacket->printDump();
    switch (lpPacket->data()->type)
    {
      case RsyncPacket::RsyncRemoteJobQuery:
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
  RsyncJob* lpJob = new RsyncJob();
  if (lpJob->descriptor().deserialize((const char*)pData, nLength))
  {
    RsyncError lStatus = RsyncSuccess;

    if (mpUserHandler)
    {
      lStatus = mpUserHandler(lpJob->descriptor());
    }
    else
    {
      lStatus = defaultQueryHandler(lpJob->descriptor());
    }

    if (lStatus == RsyncSuccess)
    {
      mJobQueue.push(lpJob);
    }

    RsyncPacket* lpPacket = new RsyncPacket(RsyncPacket::RsyncRemoteJobAcknowledgment,
                                            sizeof(lStatus), &lStatus);
    sendPacketTo(mRequestID, lpPacket);
  }
  else
  {
    delete lpJob;
  }
}

//-----------------------------------------------------------------------------
RsyncError RemoteAuthorityService::
defaultQueryHandler(JobDescriptor& rJob)
{
  RsyncError lStatus = RsyncSuccess;

  if (mrFileSys.exists(rJob.getSource()) == false)
  {
    lStatus = RsyncSourceFileNotFound;
    liber::log::error("Remote job query failed for %s\n",
                      rJob.getSource().string().c_str());
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


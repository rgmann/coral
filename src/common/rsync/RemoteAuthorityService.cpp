#include "Log.h"
#include "RsyncJob.h"
#include "RsyncPacket.h"
#include "Segment.h"
#include "RemoteAuthorityService.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
RemoteAuthorityService::RemoteAuthorityService(FileSystemInterface& rFileSystemInterface)
: PacketSubscriber()
, IThread("RemoteAuthorityService")
, mrFileSys(rFileSystemInterface)
, mAuthorityInterface(rFileSystemInterface)
, mpUserHandler(NULL)
{
  mJobQueue.initialize();
}

//-----------------------------------------------------------------------------
RemoteAuthorityService::~RemoteAuthorityService()
{
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
      case RsyncPacket::RsyncRemoteJobQuery:
        handleRemoteJobRequest(lpPacket->dataPtr(), lpPacket->data()->length);
        lbSuccess = true;
        break;

      case RsyncPacket::RsyncSegment:
        pushSegment(lpPacket->dataPtr(), lpPacket->data()->length);
        lbSuccess = true;
        break;

      default: break;
    }
  }

  return lbSuccess;
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

    sendPacket(new RsyncPacket(RsyncPacket::RsyncRemoteJobAcknowledgment,
               sizeof(lStatus), &lStatus));
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
  mAuthorityInterface.lockActiveJob();

  if (mAuthorityInterface.activeJob())
  {
    Segment* lpSegment = new Segment();
    if (lpSegment->deserialize((const char*)pData, nLength))
    {
      mAuthorityInterface.activeJob()->segments().push(lpSegment);
    }
    else
    {
      log::error("Failed to deserialize segment.\n");
      delete lpSegment;
    }
  }
  else
  {
    log::error("No job active to receive segment.\n");
  }

  mAuthorityInterface.unlockActiveJob();
}

//----------------------------------------------------------------------------
void RemoteAuthorityService::run(const bool& bShutdown)
{
  while (!bShutdown)
  {
    RsyncJob* lpJob = NULL;
    if (mJobQueue.pop(lpJob) && lpJob)
    {
      mAuthorityInterface.process(lpJob);
    }
  }
}


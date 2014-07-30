#include <new>
#include "Log.h"
#include "RsyncPacket.h"
#include "FileSystemInterface.h"
#include "RsyncJob.h"
#include "JobAgent.h"
#include "RemoteJobResult.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

typedef boost::uuids::uuid buuid;

//----------------------------------------------------------------------------
JobAgent::
JobAgent(FileSystemInterface& rFileSys, 
         SegmenterThread& rSegmenter,
         AuthorityThread& rAuthority,
         AssemblerThread& rAssembler)
: PacketSubscriber()
, mrFileSys(rFileSys)
, mrSegmenter(rSegmenter)
, mrAuthority(rAuthority)
, mrAssembler(rAssembler)
, mbCreateDestStub(true)
{
  mReadyJobs.initialize();
}

//----------------------------------------------------------------------------
JobAgent::~JobAgent()
{
}

//----------------------------------------------------------------------------
RsyncJob* JobAgent::nextJob()
{
  RsyncJob* lpJob = NULL;
  mReadyJobs.pop(lpJob);
  return lpJob;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::
createJob(const ResourcePath& destination, const ResourcePath& source, ui32 nSegmentSize)
{
  RsyncError lStatus = RsyncSuccess;
  RsyncJob*  lpJob   = NULL;

  if ((lpJob = new (std::nothrow) RsyncJob()))
  {
    JobDescriptor& rDesc = lpJob->descriptor();

    rDesc.setDestination(destination);
    rDesc.setSource(source);
    rDesc.setSegmentSize(nSegmentSize);

    if ((lStatus = createJob(lpJob)) != RsyncSuccess)
    {
      delete lpJob;
    }
  }
  else
  {
    log::error("JobAgent::createJob - Failed to allocated RsyncJob\n");
  }

  return lStatus;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::createJob(const char* pData, ui32 nSizeBytes)
{
  RsyncError lStatus = RsyncSuccess;
  RsyncJob*  lpJob   = NULL;

  if ((lpJob = new (std::nothrow) RsyncJob()))
  {
    JobDescriptor& rDesc = lpJob->descriptor();

    if (rDesc.deserialize(pData, nSizeBytes))
    {
      // Indicate that the job was remotely requested.
      rDesc.setRemoteRequest();

      // Received jobs can only specify the following combinations (table is
      // from the perspective of the RsyncNode::sync call):
      //
      // Source |  Dest  | Description
      // ---------------------------------------------------------
      // remote | remote | Tell remote node to synchronize two of its resources.
      // remote | local  | Not allowed; performed locally.
      // local  | remote | Tell remote not to synchronize its resource to local resource.
      // local  | local  | Not allowed; performed locally.
      if (rDesc.getDestination().remote)
      {
        rDesc.getDestination().remote = false;

        // If a remote JobAgent specifies a remote source, the source is now
        // local (and vice versa).
        rDesc.getSource().remote = (rDesc.getSource().remote == false);

        // Create the job.
        if ((lStatus = createJob(lpJob)) != RsyncSuccess)
        {
          delete lpJob;
        }
      }
      else
      {
        log::error("JobAgent::createJob - Bad remote job\n");
        lStatus = error(lpJob, RsyncBadRemoteJob);
      }
    }
    else
    {
      log::error("JobAgent::createJob - Bad descriptor\n");
      lStatus = error(lpJob, RsyncBadDescriptor);
    }
  }
  else
  {
    log::error("JobAgent::createJob - Failed to allocate RsyncJob\n");
  }

  return lStatus;
}

//---------------------------------------------------------------------------
RsyncError JobAgent::createJob(RsyncJob* pJob)
{
  RsyncError lStatus = RsyncSuccess;

  JobDescriptor& descriptor = pJob->descriptor();

  // If the source is local, verify that it exists.  If it does not, job
  // creation failed.
  if (descriptor.getSource().remote == false)
  {
    if (mrFileSys.exists(descriptor.getSourcePath()) == false)
    {
      log::error("JobAgent::createJob - Source resource not found\n");
      lStatus = error(pJob, RsyncSourceFileNotFound);
    }
  }

  // If the destination is local, verify that it exists. If it does not, but
  // stub creation is enabled, attempt to create it with touch. If stub
  // creation is not enabled, job creation failed.
  if (lStatus == RsyncSuccess)
  {
    if (descriptor.getDestination().remote == false)
    {
      // If the destination file does not exist, create it (if so configured).
      if (mbCreateDestStub)
      {
        if (mrFileSys.touch(descriptor.getDestinationPath()) == false)
        {
          log::error("JobAgent::createJob - Failed to touch destination resource\n");
          lStatus = error(pJob, RsyncDestinationFileNotFound);
        }
      }
      else
      {
        // If the destination still does not exist, the job cannot be performed.
        if (mrFileSys.exists(descriptor.getDestinationPath()) == false)
        {
          log::error("JobAgent::createJob - Destination resource not found\n");
          lStatus = error(pJob, RsyncDestinationFileNotFound);
        }
      }
    }
  }

  if (lStatus == RsyncSuccess)
  {
    if ((lStatus = addActiveJob(pJob)) == RsyncSuccess)
    {
      // If this is a remote job, send the descriptor to the remote node.
      //if (descriptor.isRemoteRequest())
      if (descriptor.getDestination().remote)
      {
        //log::debug("JobAgent::createJob - Sending remote job\n");
        lStatus = sendRemoteJob(descriptor);
      }
    }
  }

  lStatus = error(pJob, lStatus);

  return lStatus;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::addActiveJob(RsyncJob* pJob)
{
  RsyncError lStatus = RsyncSuccess;

  std::pair<std::map<buuid, RsyncJob*>::iterator,bool> insertStatus;
  std::pair<buuid, RsyncJob*> jobPair;

  jobPair = std::make_pair(pJob->descriptor().uuid(), pJob);
  insertStatus = mActiveJobs.insert(jobPair);

  // If the job was successfully added to the active-job table,
  // add the job to the ready job queue.
  if (insertStatus.second)
  {
    if (pJob->descriptor().getDestination().remote == false)
    {
      // If the job was successfully created, and the destination file is
      // not remote, add the job to the local pipeline.
      mrSegmenter.addJob(pJob);
      mrAuthority.addJob(pJob);
      mrAssembler.addJob(pJob);
    }

    lStatus = mReadyJobs.push(pJob) ? RsyncSuccess : RsyncQueueError;

    if (lStatus != RsyncSuccess)
    {
      log::error("JobAgent::addActiveJob - Failed to add job to queue\n");
      mActiveJobs.erase(insertStatus.first);
    }
  }
  else
  {
    log::error("JobAgent::addActiveJob - "
                   "Failed to add job to active job table\n");
    lStatus = RsyncJobTableInsertionError;
  }

  return lStatus;
}

//----------------------------------------------------------------------------
void JobAgent::removeActiveJob(RsyncJob* pJob)
{
  // Remote the job from the active job table.
  if (mActiveJobs.count(pJob->descriptor().uuid()) > 0)
  {
    mActiveJobs.erase(pJob->descriptor().uuid());
  }
  else
  {
    log::warn("JobAgent::removeActiveJob - Specified job not found\n");
  }
}

//----------------------------------------------------------------------------
RsyncError JobAgent::sendRemoteJob(const JobDescriptor& descriptor)
{
  // Send the remote job request.
  sendPacketTo(RsyncPacket::RsyncJobAgent,
               new RsyncPacket(RsyncPacket::RsyncJobRequest,
               descriptor.serialize()));

  return RsyncSuccess;
}

//----------------------------------------------------------------------------
RsyncError JobAgent::error(RsyncJob* pJob, RsyncError error)
{
  pJob->report().createJobStatus = error;

  // 
  if (pJob->descriptor().isRemoteRequest() && (error != RsyncSuccess))
  {
    RemoteJobResult result(pJob->descriptor().uuid(), pJob->report());

    sendPacketTo(RsyncPacket::RsyncJobAgent, 
                 new RsyncPacket(RsyncPacket::RsyncJobComplete,
                 result.serialize()));
  }

  return error;
}

//----------------------------------------------------------------------------
void JobAgent::releaseJob(RsyncJob* pJob)
{
  JobDescriptor& rDescriptor = pJob->descriptor();

  // Notify the remote node that a job has been completed.
  if (rDescriptor.isRemoteRequest())
  {
    RemoteJobResult result(rDescriptor.uuid(), pJob->report());

    sendPacketTo(RsyncPacket::RsyncJobAgent,
                 new RsyncPacket(RsyncPacket::RsyncJobComplete,
                 result.serialize()));
  }

  // Remote the job from the active job table.
  removeActiveJob(pJob);

  // Delete the job.
  delete pJob;
  pJob = NULL;
}

//----------------------------------------------------------------------------
bool JobAgent::put(const char* pData, ui32 nSizeBytes)
{
  bool lbSuccess = true;

  RsyncPacket packet;

  if (packet.unpack(pData, nSizeBytes))
  {
    switch (packet.data()->type)
    {
    case RsyncPacket::RsyncJobRequest:
      createJob((const char*)packet.dataPtr(), packet.data()->length);
      break;

    case RsyncPacket::RsyncJobComplete:
      finishRemoteJob((const char*)packet.dataPtr(), packet.data()->length);
      break;

    default:
      log::error("JobAgent - Unrecognized packet type");
      lbSuccess = false;
      break;
    }
  }

  return lbSuccess;
}

//----------------------------------------------------------------------------
void JobAgent::finishRemoteJob(const char* pData, ui32 nSizeBytes)
{
  RemoteJobResult jobResult;

  if (jobResult.deserialize(pData,  nSizeBytes))
  {
    if (mActiveJobs.count(jobResult.uuid()) > 0)
    {
      std::map<buuid, RsyncJob*>::iterator jobIterator;
      jobIterator = mActiveJobs.find(jobResult.uuid());

      if (jobIterator->second)
      {
        jobIterator->second->report() = jobResult.report();
        jobIterator->second->signalDone();
      }
      else
      {
        log::error("JobAgent::finishRemoteJob - NULL job\n");
      }
    }
    else
    {
      log::error("JobAgent::finishRemoteJob - Failed to find job with specified UUID\n");
    }
  }
  else
  {
    log::error("JobAgent::finishRemoteJob - Failed to deserialize job status\n");
  }
}


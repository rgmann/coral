#include "Log.h"
#include "RsyncJob.h"
#include "RsyncNode.h"

using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
RsyncNode::RsyncNode(const boost::filesystem::path& root)
: IThread("RsyncNode")
, mpCallback(NULL)
, mSegmenter(mFileSys)
, mAuthority(mFileSys)
, mAssembler(mFileSys)
, mnSegmentSize(256)
{
  mPendingJobs.initialize();
  mFileSys.setRoot(root);
  mSegmenter.launch();
  mAuthority.launch();
  mAssembler.launch();
  launch();
}

//----------------------------------------------------------------------------
RsyncNode::~RsyncNode()
{
  cancel(true);
  mSegmenter.cancel(true);
  mAuthority.cancel(true);
  mAssembler.cancel(true);
}

//----------------------------------------------------------------------------
void RsyncNode::setCompletionCallback(RsyncJobCallback* pCallback)
{
  mCallbackLock.lock();
  mpCallback = pCallback;
  mCallbackLock.unlock();
}

//----------------------------------------------------------------------------
bool RsyncNode::
sync(const boost::filesystem::path& destination,
     const boost::filesystem::path& source,
     bool bRemoteSource)
{
  RsyncJob* lpJob = new RsyncJob();

  if (lpJob)
  {
    lpJob->descriptor().setDestination(destination);
    lpJob->descriptor().setSource(source, bRemoteSource);
    lpJob->descriptor().setSegmentSize(mnSegmentSize);
    mPendingJobs.push(lpJob);
  }

  return (lpJob != NULL);
}

//----------------------------------------------------------------------------
void RsyncNode::run(const bool& bShutdown)
{
  while (!bShutdown)
  {
    RsyncJob* lpJob = NULL;

    if (mPendingJobs.pop(lpJob) && lpJob)
    {
      mSegmenter.addJob(lpJob);
      mAuthority.addJob(lpJob);
      mAssembler.addJob(lpJob);

      if (lpJob->waitDone() == false)
      {
        log::error("RsyncNode - Timeout waiting for %s job to finish.\n",
                   lpJob->descriptor().getSource().string().c_str());
      }

      mCallbackLock.lock();
      if (mpCallback)
      {
        mpCallback->call(lpJob->descriptor(), lpJob->report());
      }
      mCallbackLock.unlock();

      delete lpJob;
    }
  }
}


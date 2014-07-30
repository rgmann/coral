#include "Log.h"
#include "RsyncPacket.h"
#include "RsyncJob.h"
#include "RsyncNode.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
RsyncNode::RsyncNode(const boost::filesystem::path& root)
: IThread("RsyncNode")
, mpCallback(NULL)
, mSegmenter(mFileSys)
, mAuthority(mFileSys)
, mAssembler(mFileSys)
, mJobAgent(mFileSys, mSegmenter, mAuthority, mAssembler)
, mAuthorityService(mFileSys)
, mnSegmentSize(256)
{
  mFileSys.setRoot(root);

  mRouter.addSubscriber(RsyncPacket::RsyncAuthorityService,
                        &mAuthorityService);
  mRouter.addSubscriber(RsyncPacket::RsyncAuthorityInterface,
                        &mAuthority.getSubscriber());
  mRouter.addSubscriber(RsyncPacket::RsyncJobAgent,
                        &mJobAgent);

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

  mRouter.removeSubscriber(RsyncPacket::RsyncAuthorityService);
  mRouter.removeSubscriber(RsyncPacket::RsyncAuthorityInterface);
  mRouter.removeSubscriber(RsyncPacket::RsyncJobAgent);
}

//----------------------------------------------------------------------------
void RsyncNode::setCallback(RsyncJobCallback* pCallback)
{
  mCallbackLock.lock();
  mpCallback = pCallback;
  mCallbackLock.unlock();
}

//----------------------------------------------------------------------------
void RsyncNode::unsetCallback()
{
  mCallbackLock.lock();
  mpCallback = NULL;
  mCallbackLock.unlock();
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::
sync(const boost::filesystem::path& destinationPath,
     const boost::filesystem::path& sourcePath,
     bool bRemoteDestination, bool bRemoteSource)
{
  ResourcePath destination(destinationPath, bRemoteDestination);
  ResourcePath source(sourcePath, bRemoteSource);

  return mJobAgent.createJob(destination, source, mnSegmentSize);
}

//----------------------------------------------------------------------------
PacketSubscriber& RsyncNode::subscriber()
{
  return mRouter;
}

//----------------------------------------------------------------------------
void RsyncNode::run(const bool& bShutdown)
{
  while (!bShutdown)
  {
    RsyncJob* lpJob = NULL;

    if ((lpJob = mJobAgent.nextJob()) != NULL)
    {
      if (lpJob->waitDone() == false)
      {
        log::error("RsyncNode - Timeout waiting for %s job to finish.\n",
                   lpJob->descriptor().getSource().path.string().c_str());
      }

      mCallbackLock.lock();
      if (mpCallback)
      {
        mpCallback->call(lpJob->descriptor(), lpJob->report());
      }
      mCallbackLock.unlock();

      mJobAgent.releaseJob(lpJob);
    }
  }
}


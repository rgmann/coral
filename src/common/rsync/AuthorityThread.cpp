#include "Log.h"
#include "RsyncJob.h"
#include "AuthorityThread.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AuthorityThread::AuthorityThread(FileSystemInterface& rFileSystemInterface)
: IThread("AuthorityThread")
, mLocalAuthority(rFileSystemInterface)
{
  mJobQueue.initialize();
}

//----------------------------------------------------------------------------
AuthorityThread::~AuthorityThread()
{
}

//----------------------------------------------------------------------------
void AuthorityThread::addJob(RsyncJob* pJob)
{
  if (pJob) mJobQueue.push(pJob);
}

//----------------------------------------------------------------------------
PacketSubscriber& AuthorityThread::getSubscriber()
{
  return mRemoteAuthority;
}

//----------------------------------------------------------------------------
void AuthorityThread::setRequestID(int requestID)
{
  mRemoteAuthority.setRequestID(requestID);
}

//----------------------------------------------------------------------------
void AuthorityThread::run(const bool& bShutdown)
{
  while ( !bShutdown )
  {
    RsyncJob* lpJob = NULL;

    // Wait for a job. This is also a thread cancellation point.
    if ( mJobQueue.pop( lpJob ) && lpJob )
    {
      if (lpJob->descriptor().getSource().remote)
      {
        mRemoteAuthority.process(lpJob);
      }
      else
      {
        mLocalAuthority.process(lpJob);
      }

      lpJob->signalAuthDone();
    }
  }
}


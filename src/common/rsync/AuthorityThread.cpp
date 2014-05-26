#include "Log.h"
#include "RsyncJob.h"
#include "AuthorityThread.h"

using namespace liber;
using namespace liber::concurrency;
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
void AuthorityThread::run(const bool& bShutdown)
{
  while (!bShutdown)
  {
    RsyncJob* lpJob = NULL;

    // Wait for a job. This is also a thread cancellation point.
    if (mJobQueue.pop(lpJob) && lpJob)
    {
      if (lpJob->descriptor().remote())
      {
        mRemoteAuthority.process(lpJob);
      }
      else
      {
        log::status("Beggining local job\n");
        mLocalAuthority.process(lpJob);
      }
    }
  }
}


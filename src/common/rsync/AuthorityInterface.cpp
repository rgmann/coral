#include "AuthorityInterface.h"
#include "SegmentHash.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AuthorityInterface::AuthorityInterface()
: mpActiveJob(NULL)
{
}

//----------------------------------------------------------------------------
AuthorityInterface::~AuthorityInterface()
{
}

//----------------------------------------------------------------------------
void AuthorityInterface::process(RsyncJob* pJob)
{
  lockActiveJob();
  mpActiveJob = pJob;
  unlockActiveJob();

  processJob(activeJob());

  lockActiveJob();
  mpActiveJob = NULL;
  unlockActiveJob();
}

//----------------------------------------------------------------------------
bool AuthorityInterface::lockActiveJob()
{
  return mActiveJobLock.lock();
}

//----------------------------------------------------------------------------
void AuthorityInterface::unlockActiveJob()
{
  mActiveJobLock.unlock();
}

//----------------------------------------------------------------------------
RsyncJob* AuthorityInterface::activeJob()
{
  return mpActiveJob;
}


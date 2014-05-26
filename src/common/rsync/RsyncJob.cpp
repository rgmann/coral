#include "RsyncJob.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
RsyncJob::RsyncJob()
{
}

//-----------------------------------------------------------------------------
RsyncJob::~RsyncJob()
{
}

//-----------------------------------------------------------------------------
JobDescriptor& RsyncJob::descriptor()
{
  return mDescriptor;
}

//-----------------------------------------------------------------------------
JobReport& RsyncJob::report()
{
  return mReport;
}

//-----------------------------------------------------------------------------
SegmentQueue& RsyncJob::segments()
{
  return mSegments;
}

//-----------------------------------------------------------------------------
InstructionQueue& RsyncJob::instructions()
{
  return mInstructions;
}

//-----------------------------------------------------------------------------
void RsyncJob::signalDone()
{
  mDoneCondition.give();
}

//-----------------------------------------------------------------------------
bool RsyncJob::waitDone(int nTimeoutMs)
{
  return mDoneCondition.take(nTimeoutMs);
}


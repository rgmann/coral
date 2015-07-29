#include "Log.h"
#include "RsyncJob.h"

using namespace liber;
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
void RsyncJob::mergeReport(const JobReport& rReport)
{
  if (descriptor().getDestination().remote)
  {
    mReport.destination = rReport.destination;
  }

  if (descriptor().getSource().remote)
  {
    mReport.source = rReport.source;
  }
}

//-----------------------------------------------------------------------------
// SegmentQueue& RsyncJob::segments()
// {
//   return mSegments;
// }

//-----------------------------------------------------------------------------
InstructionQueue& RsyncJob::instructions()
{
  return mInstructions;
}

//-----------------------------------------------------------------------------
void RsyncJob::signalSegmentationDone()
{
  mSegmentationDone.give();
}

//-----------------------------------------------------------------------------
void RsyncJob::signalAuthDone()
{
  mAuthDone.give();
}

//-----------------------------------------------------------------------------
void RsyncJob::signalAssemblyDone()
{
  mAssemblyDone.give();
}

//-----------------------------------------------------------------------------
void RsyncJob::signalAllDone()
{
  mSegmentationDone.give();
  mAuthDone.give();
  mAssemblyDone.give();
}

//-----------------------------------------------------------------------------
bool RsyncJob::waitDone(int nTimeoutMs)
{
  bool lbSuccess = false;

  if (mSegmentationDone.take(nTimeoutMs))
  {
    if (mAuthDone.take(nTimeoutMs))
    {
      if ((lbSuccess = mAssemblyDone.take(nTimeoutMs)) == false)
      {
        log::error("RsyncJob::waitDone - Assembly timedout.\n");
      }
    }
    else
    {
      log::error("RsyncJob::waitDone - Authoritization timedout.\n");
    }
  }
  else
  {
    log::error("RsyncJob::waitDone - Segmentation timedout.\n");
  }

  return lbSuccess;
}


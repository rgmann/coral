#include "Log.h"
#include "RsyncJob.h"

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
RsyncJob::RsyncJob(
  FileSystemInterface&  file_sys_interface,
  RsyncPacketRouter&    router )
:  router_( router )
, file_sys_interface_( file_sys_interface )
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
void RsyncJob::mergeReport( const JobReport& report )
{
  if ( mDescriptor.getDestination().remote() )
  {
    mReport.destination = report.destination;
  }

  if ( mDescriptor.getSource().remote() )
  {
    mReport.source = report.source;
  }
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


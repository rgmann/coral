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
void RsyncJob::setDescriptor( const JobDescriptor& descriptor )
{
  mDescriptor = descriptor;
}

//-----------------------------------------------------------------------------
JobDescriptor& RsyncJob::descriptor()
{
  return mDescriptor;
}

//-----------------------------------------------------------------------------
JobReport& RsyncJob::report()
{
  return report_;
}

//-----------------------------------------------------------------------------
void RsyncJob::mergeReport( const JobReport& report )
{
  if ( mDescriptor.getDestination().remote() )
  {
    report_.destination = report.destination;
  }

  if ( mDescriptor.getSource().remote() )
  {
    report_.source = report.source;
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
  report_.destination.segmentation.complete = true;
}

//-----------------------------------------------------------------------------
void RsyncJob::signalAuthDone()
{
  mAuthDone.give();
  report_.source.segmentation.complete = true;
  report_.source.authority.complete = true;
}

//-----------------------------------------------------------------------------
void RsyncJob::signalAssemblyDone()
{
  mAssemblyDone.give();
  report_.destination.assembly.complete = true;
}

//-----------------------------------------------------------------------------
void RsyncJob::signalAllDone()
{
  mSegmentationDone.give();
  mAuthDone.give();
  mAssemblyDone.give();
}

//-----------------------------------------------------------------------------
bool RsyncJob::done() const
{
   bool job_done = true;

   if ( mDescriptor.getDestination().remote() )
   {
      if ( mDescriptor.getSource().remote() == false )
      {
         job_done =  report_.source.segmentation.complete &&
                     report_.source.authority.complete;

         // If this job was initiate locally, then we also need to have
         // received completion status from the remote node.
         if ( mDescriptor.isRemoteRequest() == false )
         {
            job_done &= report_.destination.segmentation.complete &
                        report_.destination.assembly.complete;
         }
         // log::status("RsyncJob::done: %s\n", job_done ? "DONE" : "NOT_DONE");
      }
   }
   else
   {
      job_done &= report_.destination.segmentation.complete;
      job_done &= report_.destination.assembly.complete;
      job_done &= report_.source.segmentation.complete;
      job_done &= report_.source.authority.complete;
   }

   return job_done;
}

//-----------------------------------------------------------------------------
bool RsyncJob::waitDone(int nTimeoutMs)
{
   bool job_done = false;//done();

   if ( job_done == false )
   {
      if (mSegmentationDone.take(nTimeoutMs))
      {
         if (mAuthDone.take(nTimeoutMs))
         {
            job_done = mAssemblyDone.take(nTimeoutMs);

            if ( job_done == false)
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
   }

  return job_done;
}


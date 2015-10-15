// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



#include "Log.h"
#include "RsyncJob.h"

using namespace coral;
using namespace coral::rsync;

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


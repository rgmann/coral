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
#include "Segmenter.h"
#include "FileSystemInterface.h"
#include "SegmenterThread.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::concurrency;

//----------------------------------------------------------------------------
SegmenterThread::SegmenterThread( JobAgentPairQueue& queue )
: IThread("SegmenterThread")
, job_queue_( queue )
{
}

//----------------------------------------------------------------------------
SegmenterThread::~SegmenterThread()
{
}

//----------------------------------------------------------------------------
void SegmenterThread::run(const bool& shutdown )
{
   while ( shutdown == false )
   {
      JobAgentPair job_agent_pair;

      if ( job_queue_.pop( job_agent_pair ) )
      {
         JobAgent* agent_ptr = job_agent_pair.first;
         RsyncJob* job_ptr = job_agent_pair.second;

         if ( agent_ptr && job_ptr )
         {
            std::ifstream input_file;

            bool segment_success = job_ptr->fileSystem().open(
               job_ptr->descriptor().getDestination().path(),
               input_file
            );

            if ( segment_success )
            {
               segment_success = Segmenter::processFullStride(
                  input_file,
                  job_ptr->segments(),
                  job_ptr->descriptor().getSegmentSize(),
                  job_ptr->report().destination.segmentation
               );

               if ( segment_success == false )
               {
                  log::error("SegmenterThread: %s failed\n",
                             job_ptr->descriptor().getDestination().path().string().c_str());
               }

               input_file.close();
            }
            else
            {
               log::debug("SegmenterThread: Failed to open %s\n.",
                          job_ptr->descriptor().getDestination().path().string().c_str());
            }

            job_ptr->signalSegmentationDone();
         }
      }
   }

   log::debug("SegmenterThread: Shutting down\n");
}


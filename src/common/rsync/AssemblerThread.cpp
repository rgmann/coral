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
#include "JobAgent.h"
#include "SegmentFile.h"
#include "JobDescriptor.h"
#include "FileSystemInterface.h"
#include "AssemblerThread.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AssemblerThread::AssemblerThread( JobAgentPairQueue& queue )
:  IThread( "AssemblerThread" )
,  job_queue_  ( queue )
,  assembler_  ( segment_file_ )
{
}

//----------------------------------------------------------------------------
AssemblerThread::~AssemblerThread()
{
}

//----------------------------------------------------------------------------
void AssemblerThread::run( const bool& shutdown )
{
   boost::filesystem::path stage_path_;

   while ( shutdown == false )
   {
      JobAgentPair job_agent_pair;

      if ( job_queue_.pop( job_agent_pair ) )
      {
         JobAgent* agent_ptr = job_agent_pair.first;
         RsyncJob* job_ptr   = job_agent_pair.second;

         if ( agent_ptr && job_ptr )
         {
            if ( segment_file_.open( job_ptr->fileSystem(), job_ptr->descriptor() ) )
            {
               bool stage_success = job_ptr->fileSystem().stage(
                  job_ptr->descriptor().getDestination().path(),
                  stage_path_,
                  assembler_.outputStream()
               );

               if ( stage_success )
               {
                  bool assembly_success = assembler_.process(
                     job_ptr->descriptor(),
                     job_ptr->instructions(),
                     job_ptr->report().destination.assembly
                  );

                  // Close the segment file.
                  segment_file_.close();

                  // If assembly completed successfully, swap the stage file and
                  // destination file.
                  if ( assembly_success )
                  {
                     bool swap_success = job_ptr->fileSystem().swap(
                        stage_path_,
                        job_ptr->descriptor().getDestination().path()
                     );

                     if ( swap_success == false )
                     {
                        log::error("AssemblerThread - "
                                  "Failed to swap stage file and destination file.\n");
                     }
                  }
               }
               else
               {
                  segment_file_.close();

                  log::error(
                     "AssemblerThread - Failed to open stage file for %s\n",
                     job_ptr->descriptor().getDestination().path().string().c_str());
               }
            }
            else
            {
               log::error("AssemblerThread - Failed to open segment access file.\n");
            }

            // When assembly is complete (regardless of whether is was successful),
            // signal that the RSYNC job is done.
            job_ptr->signalAssemblyDone();

            job_ptr->waitDone();
            // agent_ptr->releaseJob( job_ptr );
            agent_ptr->releaseJobIfReleasable( job_ptr );
         }
         else
         {
            log::error("AssemblerThread - NULL JobAgent or RsyncJob\n");
         }
      }
   }
}

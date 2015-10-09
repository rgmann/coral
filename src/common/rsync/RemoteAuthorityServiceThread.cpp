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
#include "RemoteAuthorityServiceThread.h"

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
RemoteAuthorityServiceThread::RemoteAuthorityServiceThread( JobAgentPairQueue& queue )
: IThread( "RemoteAuthorityServiceThread" )
, job_queue_( queue )
{
}

//-----------------------------------------------------------------------------
RemoteAuthorityServiceThread::~RemoteAuthorityServiceThread()
{
}

//-----------------------------------------------------------------------------
void RemoteAuthorityServiceThread::run( const bool& shutdown )
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
            authority_.process( job_ptr );

            if ( job_ptr->descriptor().isRemoteRequest() )
            {
               log::debug("RemoteAuthorityServiceThread::run: FINISHED REMOTE REQUEST\n");
            }
            else
            {
               log::debug("RemoteAuthorityServiceThread::run: FINISHED LOCAL REQUEST\n");
            }

            job_ptr->signalAuthDone();

            agent_ptr->releaseJobIfReleasable( job_ptr );
         }
      }
   }

   log::debug("RemoteAuthorityServiceThread: Shutting down\n");
}

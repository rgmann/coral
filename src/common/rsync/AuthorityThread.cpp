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
#include "AuthorityThread.h"
#include "PacketRouter.h"

using namespace liber;
using namespace liber::concurrency;
using namespace liber::netapp;
using namespace liber::rsync;

//----------------------------------------------------------------------------
AuthorityThread::AuthorityThread( JobAgentPairQueue& queue )
: IThread         ( "AuthorityThread" )
, job_queue_      ( queue )
{
}

//----------------------------------------------------------------------------
AuthorityThread::~AuthorityThread()
{
}

//----------------------------------------------------------------------------
PacketSubscriber& AuthorityThread::getSubscriber()
{
  return remote_authority_;
}

//----------------------------------------------------------------------------
void AuthorityThread::run( const bool& shutdown )
{
   while ( shutdown == false )
   {
      JobAgentPair job_agent_pair;

      // Wait for a job. This is also a thread cancellation point.
      if ( job_queue_.pop( job_agent_pair ) )
      {
         JobAgent* agent_ptr = job_agent_pair.first;
         RsyncJob* job_ptr   = job_agent_pair.second;

         if ( agent_ptr && job_ptr )
         {
            if ( job_ptr->descriptor().getSource().remote() )
            {
               remote_authority_.process( job_ptr );
            }
            else
            {
               local_authority_.process( job_ptr );
            }

            job_ptr->signalAuthDone();
         }
         else
         {
            log::error("AuthorityThread::run: NULL JobAgent or RsyncJob\n");
         }
      }
   }
}


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



#include "WorkerGroup.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
WorkerGroup::WorkerGroup( int worker_count )
{
   for ( int count = 0; count < worker_count; ++count )
   {
      Worker* worker = new (std::nothrow) Worker(
         segment_queue_,
         local_auth_queue_,
         assembly_queue_,
         remote_auth_queue_
      );

      if ( worker )
      {
         workers_.push_back( worker );
      }
   }
}

//-----------------------------------------------------------------------------
WorkerGroup::~WorkerGroup()
{
   std::vector<Worker*>::iterator worker = workers_.begin();
   for (; worker != workers_.end(); ++worker )
   {
      if ( *worker != NULL )
      {
         delete *worker;
         *worker = NULL;
      }
   }
}

//-----------------------------------------------------------------------------
RsyncError WorkerGroup::addJob( JobAgent* agent_ptr, RsyncJob* job_ptr )
{
   RsyncError add_status = kRsyncNoWorkersAvailable;

   if ( workers_.size() > 0 )
   {
      JobAgentPair job_agent_pair = std::make_pair( agent_ptr, job_ptr );

      if ( job_ptr->descriptor().getDestination().remote() == false )
      {
         segment_queue_.push( job_agent_pair );
         local_auth_queue_.push( job_agent_pair );
         assembly_queue_.push( job_agent_pair );
      }
      else
      {
         remote_auth_queue_.push( job_agent_pair );
      }

      add_status = kRsyncSuccess;
   }

   return add_status;
}

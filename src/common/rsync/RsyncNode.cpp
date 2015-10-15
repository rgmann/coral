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
#include "RsyncPacket.h"
#include "RsyncJob.h"
#include "RsyncNode.h"
#include "PacketRouter.h"

using namespace coral;
using namespace coral::netapp;
using namespace coral::rsync;

//----------------------------------------------------------------------------
RsyncNode::RsyncNode(
   const boost::filesystem::path& root,
   WorkerGroup& worker_group
)
:  job_agent_    ( file_sys_interface_, router_, worker_group )
{
   file_sys_interface_.setRoot(root);

   bool subscribe_success = router_.subscribe(
      RsyncPacket::RsyncJobAgent,
      &job_agent_,
      coral::netapp::kSubscriberModeReadWrite );

   if ( subscribe_success == false )
   {
      coral::log::error(
         "RsyncNode: Failed to register JobAgent as packet subscriber!\n");
   }
}

//----------------------------------------------------------------------------
RsyncNode::~RsyncNode()
{
   router_.unsubscribe( RsyncPacket::RsyncJobAgent, &job_agent_ );
}

//----------------------------------------------------------------------------
void RsyncNode::setJobCompletionCallback( RsyncJobCallback* callback_ptr )
{
   job_agent_.setJobCompletionCallback( callback_ptr );
}

//----------------------------------------------------------------------------
void RsyncNode::unsetJobCompletionCallback()
{
   job_agent_.unsetJobCompletionCallback();
}

//----------------------------------------------------------------------------
void RsyncNode::setJobRequestCallback( JobRequestCallback* callback_ptr )
{
   job_agent_.setJobRequestCallback( callback_ptr );
}

//----------------------------------------------------------------------------
void RsyncNode::unsetJobRequestCallback()
{
   job_agent_.unsetJobRequestCallback();
}

//----------------------------------------------------------------------------
bool RsyncNode::setSegmentSize( ui32 segment_size_bytes )
{
   bool limits_ok = true;

   limits_ok &= ( segment_size_bytes > 0 );
   limits_ok &= ( segment_size_bytes < 65000 );

   if ( limits_ok )
   {
      job_agent_.limits_.segment_size_bytes = segment_size_bytes;
   }

   return limits_ok;
}

//----------------------------------------------------------------------------
bool RsyncNode::setMaximumChunkSize( ui32 maximum_chunk_size_bytes )
{
   bool limits_ok = true;

   limits_ok &= ( maximum_chunk_size_bytes > 0 );
   limits_ok &= ( maximum_chunk_size_bytes < 65000 );

   if ( limits_ok )
   {
      job_agent_.limits_.maximum_chunk_size_bytes = maximum_chunk_size_bytes;
   }

   return limits_ok;
}

//----------------------------------------------------------------------------
bool RsyncNode::setCompletionTimeout( ui32 completion_timeout_ms )
{
   bool limits_ok = true;

   limits_ok &= ( completion_timeout_ms > 0 );

   if ( limits_ok )
   {
      job_agent_.limits_.completion_timeout_ms = completion_timeout_ms;
   }

   return limits_ok;
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::sync(
   const ResourcePath& destination,
   const ResourcePath& source
)
{
   return job_agent_.createJob( destination, source );
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::push( const boost::filesystem::path& filepath )
{
   ResourcePath destination( filepath, true );
   ResourcePath source( filepath, false );

   return job_agent_.createJob( destination, source );
}

//----------------------------------------------------------------------------
RsyncError RsyncNode::pull( const boost::filesystem::path& filepath )
{
   ResourcePath destination( filepath, false );
   ResourcePath source( filepath, true );

   return job_agent_.createJob( destination, source );
}

//----------------------------------------------------------------------------
PacketSubscriber& RsyncNode::subscriber()
{
   return router_;
}


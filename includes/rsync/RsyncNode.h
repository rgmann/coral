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



#ifndef RSYNC_NODE_H
#define RSYNC_NODE_H

#include "IThread.h"
#include "Queue.h"
#include "JobDescriptor.h"
#include "JobReport.h"
#include "JobAgent.h"
#include "FileSystemInterface.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"
#include "RemoteAuthorityService.h"
#include "RsyncPacketRouter.h"
#include "WorkerGroup.h"
#include "RsyncJobCallback.h"

namespace coral {
namespace rsync {

class RsyncJob;

// Active Object pattern
// - Packets are routed via the RsyncNode to Authorities
// - Requesters either have the same Router as the server or
//   they are registered with a different router based on which
//   foreign node they are syncing with.
class RsyncNode {
public:

   RsyncNode( const boost::filesystem::path& root, WorkerGroup& worker_group );
   ~RsyncNode();

   void setJobCompletionCallback( RsyncJobCallback* callback_ptr );
   void unsetJobCompletionCallback();

   void setJobRequestCallback( JobRequestCallback* callback_ptr );
   void unsetJobRequestCallback();

   bool setSegmentSize( ui32 segment_size_bytes );
   bool setMaximumChunkSize( ui32 maximum_chunk_size_bytes );
   bool setCompletionTimeout( ui32 completion_timeout_ms );

   //
   // Synchronize two files.
   //
   //
   RsyncError sync( const ResourcePath& destination, const ResourcePath& source );

   RsyncError push( const boost::filesystem::path& filepath );

   RsyncError pull( const boost::filesystem::path& filepath );

   coral::netapp::PacketSubscriber& subscriber();


private:

   RsyncPacketRouter router_;

   FileSystemInterface file_sys_interface_;
   JobAgent            job_agent_;
};

} // End namespace rsync
} // End namesapce coral

#endif // RSYNC_NODE_H


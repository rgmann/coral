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



#ifndef RSYNC_JOB_AGENT_H
#define RSYNC_JOB_AGENT_H

#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/thread/mutex.hpp>
#include "RsyncPacketSubscriber.h"
#include "JobRequestCallback.h"

namespace liber {
namespace rsync {

class RsyncJob;
class FileSystemInterface;
class RsyncPacketRouter;
class RsyncJobCallback;
class WorkerGroup;

class JobAgent : public RsyncPacketSubscriber {
public:

   JobAgent( FileSystemInterface& file_sys_interface,
            RsyncPacketRouter&   packet_router,
            WorkerGroup&         worker_group );

   ~JobAgent();

   RsyncError createJob( const ResourcePath& destination, const ResourcePath& source );

   void releaseJob( RsyncJob* job_ptr );
   void releaseJobIfReleasable( RsyncJob* job_ptr );

   RsyncJob* nextJob();

   void setJobCompletionCallback( RsyncJobCallback* callback_ptr );
   void unsetJobCompletionCallback();

   void setJobRequestCallback( JobRequestCallback* callback_ptr );
   void unsetJobRequestCallback();

   bool processPacket( const void* data_ptr, ui32 size_bytes );

   JobLimits limits_;

private:

   bool validate( const JobDescriptor& descriptor );

   void processRemoteAuthRequest( const void* data_ptr, ui32 size_bytes );

   RsyncError createJob( RsyncJob* job_ptr );
   RsyncError createJob( const char* data_ptr, ui32 size_bytes );

   RsyncError sendRemoteJob( const JobDescriptor& descriptor );

   RsyncError error( RsyncJob* job_ptr, RsyncError error );

   void finishRemoteJob(const char* data_ptr, ui32 size_bytes );

   RsyncError addActiveJob( RsyncJob* job_ptr );
   void removeActiveJob( RsyncJob* job_ptr );

   bool addJobToTable( RsyncJob* job_ptr );

private:

   typedef std::map<boost::uuids::uuid, RsyncJob*> JobTable;
   JobTable active_jobs_;
   Queue<RsyncJob*> ready_jobs_;

   FileSystemInterface& file_sys_interface_;
   RsyncPacketRouter&   packet_router_;
   WorkerGroup&         worker_group_;

   bool create_destination_stub_;

   boost::mutex callback_lock_;
   RsyncJobCallback* callback_ptr_;

   JobRequestCallback* job_request_callback_ptr_;
   JobRequestCallback default_job_request_callback_;

   boost::mutex active_jobs_lock_;
   bool allow_new_jobs_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_AGENT_H

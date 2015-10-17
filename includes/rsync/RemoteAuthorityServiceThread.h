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



#ifndef RSYNC_AUTHORITY_SERVICE_THREAD_H
#define RSYNC_AUTHORITY_SERVICE_THREAD_H

#include "IThread.h"
// #include "Queue.h"
#include "WorkerThreadTypes.h"
#include "RemoteAuthorityService.h"

namespace coral {
namespace rsync {

class RsyncJob;

class RemoteAuthorityServiceThread : public coral::thread::IThread {
public:

  RemoteAuthorityServiceThread( JobAgentPairQueue& queue );
  ~RemoteAuthorityServiceThread();

private:

  RemoteAuthorityServiceThread( const RemoteAuthorityServiceThread& );

  RemoteAuthorityServiceThread& operator= ( const RemoteAuthorityServiceThread& );

  void run(const bool& bShutdown);


private:

  // Queue<RsyncJob*> job_queue_;
  JobAgentPairQueue& job_queue_;

  RemoteAuthorityService authority_;
};

} // End namespace rsync
} // End namespace coral

#endif // RSYNC_AUTHORITY_SERVICE_THREAD_H

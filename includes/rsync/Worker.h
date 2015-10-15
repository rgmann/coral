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



#ifndef  WORKER_H
#define  WORKER_H

#include "IThread.h"
#include "WorkerThreadTypes.h"
#include "RsyncJob.h"
#include "JobAgent.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"
#include "RemoteAuthorityServiceThread.h"

namespace coral {
namespace rsync {

class Worker {
public:

  Worker(
    JobAgentPairQueue& segment_queue,
    JobAgentPairQueue& local_auth_queue,
    JobAgentPairQueue& assmebly_queue,
    JobAgentPairQueue& remote_auth_queue );

  ~Worker();

private:

  SegmenterThread segmenter_;
  AuthorityThread authority_;
  AssemblerThread assembler_;
  RemoteAuthorityServiceThread remote_authority_;
};

}
}

#endif // WORKER_H

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



#ifndef RSYNC_SEGMENTER_THREAD_H
#define RSYNC_SEGMENTER_THREAD_H

#include "IThread.h"
// #include "Queue.h"
#include "WorkerThreadTypes.h"

namespace coral {
namespace rsync {

class RsyncJob;

class SegmenterThread : public coral::concurrency::IThread {
public:

  SegmenterThread( JobAgentPairQueue& queue );
  ~SegmenterThread();

  // void addJob( RsyncJob* job_ptr );

private:

  SegmenterThread( const SegmenterThread& );

  SegmenterThread& operator= ( const SegmenterThread& );

  void run(const bool& bShutdown);


private:

  // Queue<RsyncJob*> job_queue_;
  JobAgentPairQueue& job_queue_;

  // FileSystemInterface& file_sys_interface_;
};

} // End namespace rsync
} // End namespace coral

#endif // RSYNC_SEGMENTER_THREAD_H

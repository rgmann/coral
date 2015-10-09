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



#ifndef RSYNC_REMOTE_AUTHORITY_SERVICE_H
#define RSYNC_REMOTE_AUTHORITY_SERVICE_H

#include <boost/thread/mutex.hpp>
#include "IThread.h"
#include "Queue.h"
#include "RsyncError.h"
#include "RsyncPacketSubscriber.h"
#include "FileSystemInterface.h"
#include "LocalAuthorityInterface.h"
#include "InstructionHook.h"

namespace liber {
namespace rsync {

class JobDescriptor;

class RemoteAuthorityService : public RsyncPacketSubscriber, public InstructionHook {
public:

  RemoteAuthorityService();
  ~RemoteAuthorityService();

  void setRequestID(int nRequestID);

  bool processPacket( const void* pData, ui32 nLength);

  typedef RsyncError (*QueryHandler)(JobDescriptor&);
  void setQueryHandler(QueryHandler pFunc);

  void process( RsyncJob* job_ptr );

private:

  void call(RawInstructionPtr instruction_ptr);

  void handleRemoteJobRequest(const void* pData, ui32 nLength);

  RsyncError defaultQueryHandler(JobDescriptor& rJob);

  void pushSegment(const void* pData, ui32 nLength);

private:

  LocalAuthorityInterface authority_;

  QueryHandler user_handler_ptr_;

  int request_id_;

};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_REMOTE_AUTHORITY_SERVICE_H

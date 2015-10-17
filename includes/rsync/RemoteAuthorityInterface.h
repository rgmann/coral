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


#ifndef RSYNC_REMOTE_AUTHORITY_INTERFACE_H
#define RSYNC_REMOTE_AUTHORITY_INTERFACE_H

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <fstream>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Log.h"
#include "BinarySem.h"
#include "RsyncQueryResponse.h"
#include "RsyncError.h"
#include "RsyncJob.h"
#include "RsyncPacketSubscriber.h"
#include "InstructionQueue.h"
#include "SegmentQueue.h"

namespace coral {
namespace rsync {

class RsyncPacketLite;

class RemoteAuthorityInterface : public RsyncPacketSubscriber {
public:

  RemoteAuthorityInterface();
  ~RemoteAuthorityInterface();


  void process(RsyncJob* pJob);

protected:

  bool processPacket( const void* pData, ui32 nLength);

private:

  void handleQueryResponse( const RsyncPacketLite& packet );

  RsyncError waitForEndInstruction(int nTimeoutMs);

  void releaseActiveJob();

  void sendAssemblyInstruction(const void* pData, ui32 nBytes);

  void setSourceReport(const void* pData, ui32 nBytes);

  void cancelAssembly(InstructionQueue& instructions, RsyncError status);

  RsyncError startRemoteJob(RsyncJob* pJob);

  bool flushSegments(SegmentQueue& rSegments, bool bSend);

private:

  class ActiveJob {
  public:

    ActiveJob();

    void setQueryResponse( const boost::uuids::uuid& expected_job_id, const void* data_ptr, ui32 length );

    RsyncError& queryResponse() { return query_response_; }

    void signalJobStart();

    bool waitJobStart( int timeout_ms );

    void signalJobEnd();

    bool waitJobEnd(int nTimeoutMs);

    bool timeout();

  private:

    RsyncJob* job_ptr_;

    boost::mutex job_lock_;

    thread::BinarySem job_start_signal_;
    thread::BinarySem job_end_signal_;

    RsyncError query_response_;

    // In order to detect a timeout, keep track of when the last instruction
    // was received.
    // boost::posix_time::ptime mLastInstructionTime;
  } active_job_;

  int mnSegmentTimeoutMs;
  boost::posix_time::ptime last_instruction_time_;

};

} // End namespace rsync
} // End namespace coral

#endif // RSYNC_REMOTE_AUTHORITY_INTERFACE_H

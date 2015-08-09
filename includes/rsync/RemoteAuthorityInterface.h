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
#include "PacketSubscriber.h"
#include "InstructionQueue.h"
#include "SegmentQueue.h"

namespace liber {
namespace rsync {

class RemoteAuthorityInterface : public liber::netapp::PacketSubscriber {
public:

  RemoteAuthorityInterface();
  ~RemoteAuthorityInterface();

  bool put(const char* pData, ui32 nLength);


  void process(RsyncJob* pJob);

private:

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

    static const i64 JOB_TIMEOUT_MS = 2000;

    ActiveJob();

    void setJob( RsyncJob* job_ptr );
    RsyncJob* job() { return job_ptr_; };

    void setQueryResponse( const void* data_ptr, ui32 length );

    RsyncError& queryResponse() { return query_response_; }

    void signalJobStart();

    bool waitJobStart( int timeout_ms );

    void signalJobEnd();

    bool waitJobEnd(int nTimeoutMs);

    bool lockIfActive();

    void unlock() { job_lock_.unlock(); };

    void pushInstruction( Instruction* pInstruction );

    bool timeout();

  private:

    RsyncJob* job_ptr_;

    boost::mutex job_lock_;

    BinarySem job_start_signal_;
    BinarySem job_end_signal_;

    RsyncError query_response_;

    // In order to detect a timeout, keep track of when the last instruction
    // was received.
    boost::posix_time::ptime mLastInstructionTime;
  } active_job_;

  int mnSegmentTimeoutMs;

  // int mRequestID;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_REMOTE_AUTHORITY_INTERFACE_H

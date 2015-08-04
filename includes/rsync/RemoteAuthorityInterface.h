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

  // void setRequestID(int requestID);

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

    ActiveJob() : mpJob(NULL), mQueryResponse(RsyncSuccess) {}

    inline void setJob(RsyncJob* pJob) {
      mpJob = pJob;
      mLastInstructionTime = boost::posix_time::microsec_clock::local_time();
    };
    inline RsyncJob* job() { return mpJob; };

    void setQueryResponse( const void* data_ptr, ui32 length ) {
      RsyncQueryResponse response;
      if ( response.deserialize( (const char*)data_ptr, length ) )
      {
        if ( response.uuid() == mpJob->descriptor().uuid() )
        {
          mQueryResponse = response.status();
        }
        else
        {
          mQueryResponse = kRsyncRemoteQueryError;
          liber::log::error("RemoteAuthorityInterface::ActiveJob::setQueryResponse: Invalid UUID\n");
        }
      }
      else {
        mQueryResponse = kRsyncRemoteQueryError;
        liber::log::error("RemoteAuthorityInterface::ActiveJob::setQueryResponse: Failed to deserialize\n");
      }

      mJobStartSignal.give();
    }

    inline RsyncError& queryResponse() { return mQueryResponse; }

    inline void signalJobStart()
    {
      mJobStartSignal.give();
    }

    bool waitJobStart(int nTimeoutMs)
    {
      liber::thread::Semaphore::SemStatus lStatus = mJobStartSignal.take(nTimeoutMs);
      return (lStatus == liber::thread::Semaphore::SemAcquired);
    }

    inline void signalJobEnd()
    {
      mJobEndSignal.give();
    }

    inline bool waitJobEnd(int nTimeoutMs)
    {
      return (mJobEndSignal.take(nTimeoutMs) == liber::thread::Semaphore::SemAcquired);
    }

    bool lockIfActive()
    {
      job_lock_.lock();
      bool lbActive = (mpJob != NULL);
      if (!lbActive) job_lock_.unlock();
      return lbActive;
    }

    inline void unlock() { job_lock_.unlock(); };

    void pushInstruction( Instruction* pInstruction );

    bool timeout()
    {
      boost::posix_time::time_duration deltaTime =
        boost::posix_time::microsec_clock::local_time() - mLastInstructionTime;
      return ( deltaTime.total_milliseconds() >= JOB_TIMEOUT_MS );
    }

  private:

    RsyncJob* mpJob;

    boost::mutex job_lock_;

    BinarySem mJobStartSignal;
    BinarySem mJobEndSignal;

    RsyncError mQueryResponse;

    // In order to detect a timeout, keep track of when the last instruction
    // was received.
    boost::posix_time::ptime mLastInstructionTime;
  } mActiveJob;

  int mnSegmentTimeoutMs;
  int mnJobAckTimeoutMs;
  int mnJobCompletionTimeoutMs;

  // int mRequestID;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_REMOTE_AUTHORITY_INTERFACE_H

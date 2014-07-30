#ifndef RSYNC_REMOTE_AUTHORITY_INTERFACE_H
#define RSYNC_REMOTE_AUTHORITY_INTERFACE_H

#include <fstream>
#include "BinarySem.h"
#include "AuthorityInterface.h"
#include "RsyncError.h"
#include "PacketSubscriber.h"
#include "InstructionQueue.h"
#include "SegmentQueue.h"

namespace liber {
namespace rsync {

class RemoteAuthorityInterface
//: public AuthorityInterface
: public liber::netapp::PacketSubscriber {
public:

  RemoteAuthorityInterface();
  ~RemoteAuthorityInterface();

  void setRequestID(int requestID);

  bool put(const char* pData, ui32 nLength);


  void process(RsyncJob* pJob);

private:

//  void processJob(RsyncJob* pJob);

  RsyncError waitForEndInstruction(int nTimeoutMs);

  void releaseActiveJob();

  void sendAssemblyInstruction(const void* pData, ui32 nLength);

  void cancelAssembly(InstructionQueue& instructions, RsyncError status);

  RsyncError startRemoteJob(RsyncJob* pJob);

  void flushSegments(SegmentQueue& rSegments, bool bSend);

private:

  class ActiveJob {
  public:

    ActiveJob() : mpJob(NULL), mQueryResponse(RsyncSuccess) {}

    inline void setJob(RsyncJob* pJob) { mpJob = pJob; };
    inline RsyncJob* job() { return mpJob; };

    inline RsyncError& queryResponse() { return mQueryResponse; }

    inline void signalJobStart()
    {
      mJobStartSignal.give();
    }

    bool waitJobStart(int nTimeoutMs)
    {
      Sem::SemStatus lStatus = mJobStartSignal.take(nTimeoutMs);
      return (lStatus == Sem::SemAcquired);
    }

    inline void signalJobEnd()
    {
      mJobEndSignal.give();
    }

    inline bool waitJobEnd(int nTimeoutMs)
    {
      return (mJobEndSignal.take(nTimeoutMs) == Sem::SemAcquired);
    }

    bool lockIfActive()
    {
      mJobLock.lock();
      bool lbActive = (mpJob != NULL);
      if (!lbActive) mJobLock.unlock();
      return lbActive;
    }

    inline void unlock() { mJobLock.unlock(); };

  private:

    RsyncJob* mpJob;

    Mutex mJobLock;

    BinarySem mJobStartSignal;
    BinarySem mJobEndSignal;

    RsyncError mQueryResponse;
  } mActiveJob;

  int mnSegmentTimeoutMs;
  int mnJobAckTimeoutMs;
  int mnJobCompletionTimeoutMs;

  int mRequestID;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_REMOTE_AUTHORITY_INTERFACE_H

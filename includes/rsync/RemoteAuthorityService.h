#ifndef RSYNC_REMOTE_AUTHORITY_SERVICE_H
#define RSYNC_REMOTE_AUTHORITY_SERVICE_H

#include <boost/thread/mutex.hpp>
#include "IThread.h"
#include "Queue.h"
#include "RsyncError.h"
#include "PacketSubscriber.h"
// #include "AuthorityInterface.h"
#include "FileSystemInterface.h"
#include "LocalAuthorityInterface.h"
#include "InstructionHook.h"

namespace liber {
namespace rsync {

class JobDescriptor;

class JobCompletionHook {
public:
  virtual ~JobCompletionHook() {};
  virtual void operator () (RsyncJob* pJob) = 0;
};

class JobQueue : public liber::concurrency::IThread {
public:

  JobQueue(FileSystemInterface&, InstructionReceiver&, JobCompletionHook&);


  RsyncJob* activeJob();
  boost::mutex& lock();
  bool lockIfActive();

  bool push(RsyncJob* pJob);

private:

  void run(const bool& bShutdown);


private:

  LocalAuthorityInterface mAuthority;
  InstructionReceiver& mrReceiver;

  JobCompletionHook& mrJobHook;

  Queue<RsyncJob*> mJobQueue;

  RsyncJob* mpActiveJob;
  boost::mutex job_lock_;
};

class RemoteAuthorityService
: public liber::netapp::PacketSubscriber
, public InstructionHook {
public:

  RemoteAuthorityService(FileSystemInterface&);
  ~RemoteAuthorityService();

  void setRequestID(int nRequestID);

  bool put(const char* pData, ui32 nLength);

  typedef RsyncError (*QueryHandler)(JobDescriptor&);
  void setQueryHandler(QueryHandler pFunc);

private:

  void call(Instruction* pInstruction);

  void handleRemoteJobRequest(const void* pData, ui32 nLength);

  RsyncError defaultQueryHandler(JobDescriptor& rJob);

  void pushSegment(const void* pData, ui32 nLength);

private:

  FileSystemInterface& mrFileSys;

  QueryHandler mpUserHandler;

  int mRequestID;


  class SendReportHook : public JobCompletionHook {
  public:
    SendReportHook(liber::netapp::PacketSubscriber& rSubscriber);
    ~SendReportHook() {};

    void setRequestID(int requestID);
    void operator () (RsyncJob* pJob);

  private:
    liber::netapp::PacketSubscriber& mrSubscriber;
    int mRequestID;
  } mSendReportHook;

  JobQueue mJobQueue;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_REMOTE_AUTHORITY_SERVICE_H

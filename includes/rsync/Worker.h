#ifndef  WORKER_H
#define  WORKER_H

// #include "Queue.h"
#include "IThread.h"
#include "WorkerThreadTypes.h"
#include "RsyncJob.h"
#include "JobAgent.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"
#include "RemoteAuthorityServiceThread.h"

namespace liber {
namespace rsync {

// typedef std::pair<JobAgent*,RsyncJob*>  JobAgentPair;
// typedef Queue<JobAgentPair>             JobAgentPairQueue;

class Worker : liber::concurrency::IThread {
public:

  Worker( Queue<JobAgentPair>& job_queue );
  ~Worker();

private:

  void run( const bool& shutdown );


private:

  Queue<JobAgentPair>& job_queue_;

  SegmenterThread segmenter_;
  AuthorityThread authority_;
  AssemblerThread assembler_;

  RemoteAuthorityService authority_service_;

};

class PassiveWorker {
public:

  PassiveWorker(
    JobAgentPairQueue& segment_queue,
    JobAgentPairQueue& local_auth_queue,
    JobAgentPairQueue& assmebly_queue,
    JobAgentPairQueue& remote_auth_queue );

  ~PassiveWorker();

private:

  SegmenterThread segmenter_;
  AuthorityThread authority_;
  AssemblerThread assembler_;
  RemoteAuthorityServiceThread remote_authority_;
};

}
}

#endif // WORKER_H

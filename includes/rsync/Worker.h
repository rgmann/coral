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

namespace liber {
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

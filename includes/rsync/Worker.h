#ifndef  WORKER_H
#define  WORKER_H

#include "Queue.h"
#include "IThread.h"
#include "RsyncJob.h"
#include "JobAgent.h"
#include "SegmenterThread.h"
#include "AuthorityThread.h"
#include "AssemblerThread.h"
#include "RemoteAuthorityService.h"

namespace liber {
namespace rsync {

typedef std::pair<JobAgent*,RsyncJob*>  JobAgentPair;

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

}
}

#endif // WORKER_H

#ifndef  WORKER_GROUP_H
#define  WORKER_GROUP_H

#include <vector>
#include "Worker.h"

namespace liber {
namespace rsync {

class WorkerGroup {
public:

  WorkerGroup( int worker_count = 1 );
  ~WorkerGroup();

  RsyncError addJob( JobAgent* job_agent_ptr, RsyncJob* job_ptr );

private:

  std::vector<Worker*> workers_;

  Queue<JobAgentPair> job_queue_;

};

}
}

#endif // WORKER_GROUP_H
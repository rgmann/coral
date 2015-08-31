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

   const std::vector<Worker*>& workers() const {  return workers_; }

private:

   std::vector<Worker*> workers_;

   JobAgentPairQueue segment_queue_;
   JobAgentPairQueue local_auth_queue_;
   JobAgentPairQueue assembly_queue_;
   JobAgentPairQueue remote_auth_queue_;

};

} // namespace rsync
} // namesapce liber

#endif // WORKER_GROUP_H
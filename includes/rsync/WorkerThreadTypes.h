#ifndef  WORKER_THREAD_TYPES_H
#define  WORKER_THREAD_TYPES_H

#include <utility>
#include "Queue.h"


namespace liber {
namespace rsync {
class JobAgent;
class RsyncJob;
typedef std::pair<JobAgent*,RsyncJob*> JobAgentPair;
typedef Queue<JobAgentPair> JobAgentPairQueue;
}
}

#endif // WORKER_THREAD_TYPES_H
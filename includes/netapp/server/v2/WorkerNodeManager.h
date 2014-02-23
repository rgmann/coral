#ifndef WORKER_NODE_MANAGER_H
#define WORKER_NODE_MANAGER_H

#include <vector>
#include "Queue.h"
#include "IThread.h"
#include "ApplicationWorker.h"
#include "WorkerNode.h"

namespace liber  {
namespace netapp {

class WorkerNodeManager : public liber::concurrency::IThread {
public:

  WorkerNodeManager();
  ~WorkerNodeManager();

  bool addWorker(ApplicationWorker* pWorker);

  using liber::concurrency::IThread::launch;
  bool launch(WorkerNodeComparator::Mode mode);

private:

  void spawnNode();
  void distributeWorker(ApplicationWorker* pWorker);
  void destroyNodes();

  void run(const bool& bShutdown);

private:

  WorkerNodeComparator      mComparator;
  Queue<ApplicationWorker*> mBalanceQueue;
  std::vector<WorkerNode*>  mNodes;
};

} // End namespace netapp
} // End namespace liber

#endif // WORKER_NODE_MANAGER_H


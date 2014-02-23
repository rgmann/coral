#ifndef WORKER_NODE_H
#define WORKER_NODE_H

#include <vector>
#include "IThread.h"
#include "NodeInputThread.h"
#include "NodeOutputThread.h"
#include "Timestamp.h"

namespace liber {
namespace netapp {


class WorkerNode : public liber::concurrency::IThread {
public:

  WorkerNode();
  virtual ~WorkerNode();

  void addWorker(ApplicationWorker* pWorker);

  const Timestamp& getLaunchTime() const;
  ui32 getWorkerCount() const;

  bool launch();

private:

  void insertWorker(ApplicationWorker* pWorker);

  /**
   * Adds new workers, removes inactive workers, tracks performance stats.
   */
  void run(const bool& bShutdown);

private:

  Timestamp mLaunchTime;

  WorkerRemovalAgent mRemover;

  NodeInputThread  mInThread;
  NodeOutputThread mOutThread;

  Queue<ApplicationWorker*> mNewWorkers;
  std::vector<Worker*>      mWorkers;
};


class WorkerNodeComparator {
public:

  enum Mode {
    LastAddFirst,
    FewestWorkersFirst,
    MostWorkersFirst
  };

  WorkerNodeComparator(Mode mode = FewestWorkersFirst);

  void setMode(Mode mode);

  bool operator() (const WorkerNode* lhs, const WorkerNode* rhs);

private:

  Mode mMode;
};

} // End namespace netapp
} // End namespace liber

#endif // WORKER_NODE_H


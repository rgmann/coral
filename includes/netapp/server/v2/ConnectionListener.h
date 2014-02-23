#ifndef CONNECTION_LISTENER_H
#define CONNECTION_LISTENER_H

#include "IThread.h"
#include "ApplicationWorker.h"
#include "WorkerNodeManager.h"

namespace liber  {
namespace netapp {

class WorkerCreator {
public:

  WorkerCreator();
  virtual ~WorkerCreator();

  virtual ApplicationWorker* create() = 0;
};


class ConnectionListener : public liber::concurrency::IThread {
public:

  ConnectionListener(WorkerNodeManager& rNodeManager);
  ~ConnectionListener();

  using liber::concurrency::IThread::launch;
  bool launch(int nPort, WorkerCreator* pCreator);

private:

  void run(const bool& bShutdown);

private:

  int mnPort;
  WorkerCreator* mpCreator;

  WorkerNodeManager& mrNodeManager;
};

} // End namespace netapp
} // End namespace liber

#endif // CONNECTION_LISTENER_H


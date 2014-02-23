#ifndef APPLICATION_SERVER_H
#define APPLICATION_SERVER_H

#include "WorkerNodeManager.h"
#include "ConnectionListener.h"

namespace liber  {
namespace netapp {


class ApplicationServer {
public:

  ApplicationServer();
  ~ApplicationServer();

  bool start(int nPort, WorkerCreator* pCreator,
             WorkerNodeComparator::Mode mode = WorkerNodeComparator::FewestWorkersFirst);

  bool stop();

private:

  WorkerNodeManager   mNodeManager;

  // ConnectionListener is application specific because it needs to know
  // how to construct an application specific worker?
  ConnectionListener mListener;
};

} // End namespace netapp
} // End namespace liber

#endif // APPLICATION_SERVER_H

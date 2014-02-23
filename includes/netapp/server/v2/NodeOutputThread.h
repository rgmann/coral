#ifndef NODE_OUTPUT_THREAD_H
#define NODE_OUTPUT_THREAD_H

#include "ServerWorkerThread.h"

namespace liber {
namespace netapp {

// NodeOutputThread gives each queued worker a chance to send
// a packet.

class NodeOutputThread : public netapp::ServerWorkerThread {
public:

  NodeOutputThread();

private:

  bool processWorker(Worker* pWorker);

private:

  ui32 mnMaxRotationPeriodMs;
};

}}

#endif // NODE_OUTPUT_THREAD_H


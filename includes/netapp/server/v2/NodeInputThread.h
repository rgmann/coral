#ifndef NODE_INPUT_THREAD_H
#define NODE_INPUT_THREAD_H

#include "BaseTypes.h"
#include "ServerWorkerThread.h"

namespace liber {
namespace netapp {

// NodeInputThread gives each queued worker a chance to receive
// a packet.  If a packet is received, it is also routed.

class NodeInputThread : public netapp::ServerWorkerThread {
public:

  NodeInputThread();

  static const ui32 ourDefaultReadTimeoutMs = 100;
  void setReadTimeout(ui32 nTimeoutMs);

private:

  bool processWorker(Worker* pWorker);

private:

  ui32 mnReadTimeoutMs;
};

} // End namespace netapp
} // End namespace liber

#endif // NODE_INPUT_THREAD_H


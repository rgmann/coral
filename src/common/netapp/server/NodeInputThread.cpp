#include "NodeInputThread.h"

using namespace liber;
using namespace netapp;

//-----------------------------------------------------------------------------
NodeInputThread::NodeInputThread()
: ServerWorkerThread("NodeInputThread")
, mnReadTimeoutMs(ourDefaultReadTimeoutMs)
{
}

//-----------------------------------------------------------------------------
bool NodeInputThread::processWorker(Worker* pWorker)
{
  bool lbWorkerAlive = true;
  lbWorkerAlive &= pWorker->workerPtr()->readPacket(mnReadTimeoutMs, true);
//  lbWorkerAlive &= pWorker->workerPtr()->status().isActive();
  return lbWorkerAlive;
}



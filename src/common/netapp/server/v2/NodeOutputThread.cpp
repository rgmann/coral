#include "NodeOutputThread.h"

using namespace liber;
using namespace netapp;

//-----------------------------------------------------------------------------
NodeOutputThread::NodeOutputThread()
: ServerWorkerThread("NodeOutputThread")
, mnMaxRotationPeriodMs(100)
{
}

//-----------------------------------------------------------------------------
bool NodeOutputThread::processWorker(Worker* pWorker)
{
  ui32 lnServiceTimeMs = mnMaxRotationPeriodMs;
  ui32 lnPendingWorkers = pendingJobCount();

  if (lnPendingWorkers > 0)
  {
    lnServiceTimeMs /= pendingJobCount();
  }
 
  return pWorker->workerPtr()->writePacket(lnServiceTimeMs);
}



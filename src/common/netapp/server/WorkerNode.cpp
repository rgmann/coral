#include "Log.h"
#include "WorkerNode.h"

using namespace liber;
using namespace liber::net;
using namespace liber::concurrency;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
WorkerNode::WorkerNode()
: IThread("WorkerNode")
{
  // mNewWorkers.initialize();
}

//-----------------------------------------------------------------------------
WorkerNode::~WorkerNode()
{
}

//-----------------------------------------------------------------------------
void WorkerNode::addWorker(ApplicationWorker* pWorker)
{
  mNewWorkers.push(pWorker);
}

//-----------------------------------------------------------------------------
const Timestamp& WorkerNode::getLaunchTime() const
{
  return mLaunchTime;
}

//-----------------------------------------------------------------------------
ui32 WorkerNode::getWorkerCount() const
{
  return mWorkers.size();
}

//-----------------------------------------------------------------------------
bool WorkerNode::launch()
{
  bool lbSuccess = true;

  mLaunchTime = Timestamp::Now();

  lbSuccess &= mInThread.launch();
  lbSuccess &= mOutThread.launch();
  lbSuccess &= IThread::launch();

  return lbSuccess;
}

//-----------------------------------------------------------------------------
void WorkerNode::insertWorker(ApplicationWorker* pWorker)
{
  log::debug("WorkerNode: Inserting worker.\n");
  Worker* lpNewWorker = new Worker(pWorker, mRemover);
  mWorkers.push_back(lpNewWorker);
  mInThread.addWorker(lpNewWorker);
  mOutThread.addWorker(lpNewWorker);
}

//-----------------------------------------------------------------------------
void WorkerNode::run(const bool& bShutdown)
{
  while (true)
  {
    if (bShutdown)
    {
      // As part of the shutdown process, the threads will unreference the
      // workers. When the reference count for each worker reaches zero, a
      // removal request is sent to the WorkerRemovalFunctor. Removal requests
      // are processed below before the thread loop exits.
      mInThread.shutdown();
      mOutThread.shutdown();
    }
    else
    {
      ApplicationWorker* lpWorker = NULL;
      if (mNewWorkers.pop(lpWorker, 500) && lpWorker)
      {
        insertWorker(lpWorker);
      }
    }

    // Process removal requests.
    ApplicationWorker* lpWorkerToRemove = NULL;
    while ((lpWorkerToRemove = mRemover.getRemovableWorker(0)))
    {
      std::vector<Worker*>::iterator lWorkerIt = mWorkers.begin();
      for (; lWorkerIt != mWorkers.end(); lWorkerIt++)
      {
        if (lpWorkerToRemove == (*lWorkerIt)->workerPtr())
        {
          // Erase the worker from the Worker vector.
          delete *lWorkerIt;
          *lWorkerIt = NULL;
          mWorkers.erase(lWorkerIt);

          // Break out of the loop since the worker was found.
          break;
        }
      }

      lpWorkerToRemove = NULL;
    }

    if (bShutdown)
    {
      break;
    }
  }
}



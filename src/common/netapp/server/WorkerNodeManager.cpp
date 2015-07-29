#include "Log.h"
#include "WorkerNodeManager.h"

using namespace liber;
using namespace netapp;

//-----------------------------------------------------------------------------
WorkerNodeManager::WorkerNodeManager()
: IThread("WorkerNodeManager")
{
  // mBalanceQueue.initialize();
}

//-----------------------------------------------------------------------------
WorkerNodeManager::~WorkerNodeManager()
{
  if (isRunning())
  {
    join();
  }

  destroyNodes();
}

//-----------------------------------------------------------------------------
bool WorkerNodeManager::addWorker(ApplicationWorker* pWorker)
{
  return mBalanceQueue.push(pWorker);
}

//-----------------------------------------------------------------------------
bool WorkerNodeManager::launch(WorkerNodeComparator::Mode mode)
{
  mComparator.setMode(mode);
  return IThread::launch();
}

//-----------------------------------------------------------------------------
void WorkerNodeManager::run(const bool& bShutdown)
{
  ApplicationWorker* lpWorker = NULL;

  log::status("WorkerNodeManager: Started\n");

  while (!bShutdown)
  {
    if (mBalanceQueue.pop(lpWorker, 1000) && lpWorker)
    {
      distributeWorker(lpWorker);
      lpWorker = NULL;
    }
  }

  log::status("WorkerNodeManager: Shutting down\n");
}

//-----------------------------------------------------------------------------
void WorkerNodeManager::distributeWorker(ApplicationWorker* pWorker)
{
  if (mNodes.empty())
  {
    spawnNode();
  }

  log::debug("WorkerNodeManager: Distributing worker.\n");
  std::sort(mNodes.begin(), mNodes.end(), mComparator);

  WorkerNode* lpNode = mNodes.front();
  if (lpNode)
  {
    lpNode->addWorker(pWorker);
  }
}

//-----------------------------------------------------------------------------
void WorkerNodeManager::spawnNode()
{
  log::debug("WorkerNodeManager: Spawning new worker node.\n");
  WorkerNode* lpNode = new WorkerNode();
  lpNode->launch();
  mNodes.push_back(lpNode);
}

//-----------------------------------------------------------------------------
void WorkerNodeManager::destroyNodes()
{
  while (!mNodes.empty())
  {
    WorkerNode* lpNode = mNodes.back();
    if (lpNode)
    {
      delete lpNode;
      lpNode = NULL;
    }

    mNodes.pop_back();
  }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
WorkerNodeComparator::WorkerNodeComparator(Mode mode)
: mMode(mode)
{
}

//-----------------------------------------------------------------------------
void WorkerNodeComparator::setMode(Mode mode)
{
  mMode = mode;
}

//-----------------------------------------------------------------------------
bool WorkerNodeComparator
::operator() (const WorkerNode* lhs, const WorkerNode* rhs)
{
  bool lbGreaterThan = false;

  switch (mMode)
  {
  case LastAddFirst:
    lbGreaterThan = (lhs->getLaunchTime() < rhs->getLaunchTime());
    break;

  case FewestWorkersFirst:
    lbGreaterThan = (lhs->getWorkerCount() <= rhs->getWorkerCount());
    break;

  case MostWorkersFirst:
    lbGreaterThan = (lhs->getWorkerCount() >= rhs->getWorkerCount());
    break;
  }

  return lbGreaterThan;
}



#include "Log.h"
#include "ServerWorkerThread.h"

using namespace liber::netapp;

//-----------------------------------------------------------------------------
// Worker Class Implmentation
//-----------------------------------------------------------------------------
WorkerRemovalAgent::WorkerRemovalAgent()
{
  mWorkersToRemove.initialize();
}

//-----------------------------------------------------------------------------
void WorkerRemovalAgent::requestRemoval(ApplicationWorker* pWorker)
{
  mWorkersToRemove.push(pWorker);
}

//-----------------------------------------------------------------------------
ApplicationWorker* WorkerRemovalAgent::getRemovableWorker(int nTimeoutMs)
{
  ApplicationWorker* lpWorker = NULL;
  mWorkersToRemove.pop(lpWorker, nTimeoutMs);
  return lpWorker;
}

//-----------------------------------------------------------------------------
// Worker Class Implmentation
//-----------------------------------------------------------------------------
Worker::Worker(ApplicationWorker* pWorker, WorkerRemovalAgent& rRemover)
: mrRemover(rRemover)
, mpWorker(pWorker)
, mnReferenceCount(0)
{
}

//-----------------------------------------------------------------------------
Worker::~Worker()
{
  if (mpWorker)
  {
    delete mpWorker;
  }
}

//-----------------------------------------------------------------------------
bool Worker::inactive()
{
  mCopyLock.lock();
  bool lbInactive = (mnReferenceCount == 0);
  mCopyLock.unlock();
  return lbInactive;
}

//-----------------------------------------------------------------------------
bool Worker::valid() const
{
  return (mpWorker != NULL);
}

//-----------------------------------------------------------------------------
ApplicationWorker* Worker::workerPtr()
{
  return mpWorker;
}

//-----------------------------------------------------------------------------
Worker& Worker::operator++ (int unused)
{
  mCopyLock.lock();
  mnReferenceCount++;
  mCopyLock.unlock();

  return *this;
}

//-----------------------------------------------------------------------------
Worker& Worker::operator-- (int unused)
{
  mCopyLock.lock();
  mnReferenceCount--;
  if (mnReferenceCount == 0)
  {
    mrRemover.requestRemoval(mpWorker);
  }
  mCopyLock.unlock();

  return *this;
}


//-----------------------------------------------------------------------------
// ServerWorkerThread Class Implementation
//-----------------------------------------------------------------------------
ServerWorkerThread::ServerWorkerThread(const std::string& name)
: IThread(name)
{
  mWorkers.initialize();
}

//-----------------------------------------------------------------------------
ServerWorkerThread::~ServerWorkerThread()
{
}

//-----------------------------------------------------------------------------
void ServerWorkerThread::addWorker(Worker* pWorker)
{
  // Increment the reference count.
  (*pWorker)++;

  // Enqueue the worker.
  mWorkers.push(pWorker);

  log::debug("%s: adding worker.\n", getName().c_str());
}

//-----------------------------------------------------------------------------
void ServerWorkerThread::flush()
{
  Worker* lpWorker = NULL;

  while (mWorkers.pop(lpWorker, 100))
  {
    if (lpWorker)
    {
      // Decrement the number of references.
      (*lpWorker)--;
    }

    lpWorker = NULL;
  }
}

//-----------------------------------------------------------------------------
void ServerWorkerThread::shutdown()
{
  join();
  flush();
}

//-----------------------------------------------------------------------------
ui32 ServerWorkerThread::pendingJobCount()
{
  return mWorkers.size();
}

//-----------------------------------------------------------------------------
void ServerWorkerThread::releaseWorker(Worker* pWorker)
{
  log::debug("%s: releasing worker.\n", getName().c_str());

  // Decrement the number of references to the Worker.
  (*pWorker)--;
}

//-----------------------------------------------------------------------------
void ServerWorkerThread::run(const bool& bShutdown)
{
  Worker* lpWorker = NULL;

  while (!bShutdown)
  {
    if (mWorkers.pop(lpWorker, 500) && lpWorker)
    {
      if (processWorker(lpWorker))
      {
        mWorkers.push(lpWorker);
      }
      else
      {
        releaseWorker(lpWorker);
      }
    }

    lpWorker = NULL;
  }
}



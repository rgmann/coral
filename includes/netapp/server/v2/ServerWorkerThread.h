#ifndef SERVER_WORKER_THREAD_H
#define SERVER_WORKER_THREAD_H

#include "Queue.h"
#include "IThread.h"
#include "ApplicationWorker.h"

namespace liber  {
namespace netapp {


class WorkerRemovalAgent {
public:

  WorkerRemovalAgent();

  void requestRemoval(ApplicationWorker* pWorker);

  ApplicationWorker* getRemovableWorker(int nTimeoutMs);

private:

  Queue<ApplicationWorker*> mWorkersToRemove;
};


class Worker {
public:

  Worker(ApplicationWorker* pWorker, WorkerRemovalAgent& rRemover);
  ~Worker();

  /**
   * Returns true if the worker has been released by WorkerNode's send and
   * receive threads.  An inactive Worker is safe to destroy.
   */
  bool inactive();

  /**
   * A worker is only valid if its ApplicationWorker pointer is set.
   */
  bool valid() const;

  ApplicationWorker* workerPtr();

  Worker& operator++ (int unused);
  Worker& operator-- (int unused);

private:

  WorkerRemovalAgent& mrRemover;

  Mutex mCopyLock;

  ApplicationWorker* mpWorker;

  // A worker may be reference from several threads.  The Worker thread
  // is inactive, when the reference count is zero.
  unsigned int mnReferenceCount;
};


class ServerWorkerThread : public liber::concurrency::IThread {
public:

  ServerWorkerThread(const std::string& name = "");
  virtual ~ServerWorkerThread();

  virtual void addWorker(Worker* pWorker);

  /**
   * Release all workers in the queue.  flush() is called in the
   * destructor.
   */
  virtual void flush();

  void shutdown();

  struct Statistics {
    ui64 liveTimeMs;
    ui64 busyTimeMs;
    ui64 idleTimeMs;
  };
  Statistics getStatistics() const;

  /**
   * Returns the threads utilization as "busy time" / "live time".
   */
  f32 getUtilization() const;

  /**
   * Count the number of waiting workers.
   */
  ui32 pendingJobCount();

protected:

  /**
   * The server worker thread performs the same processing for each worker,
   * whether is receiving, sending, or doing some other task.  If the task
   * fails such that the worker should not repeat the task in the future,
   * processWorker should return false. This notifies the ServerWorkerThread
   * to release the worker.
   */
  virtual bool processWorker(Worker* pWorker) = 0;

  virtual void releaseWorker(Worker* pWorker);

  virtual void run(const bool& bShutdown);

protected:

  Timestamp mLaunchTime;
  Timestamp mIdleTime;
  Timestamp mBusyTime;

  Queue<Worker*> mWorkers;
};

} // End namespace netapp
} // End namespace liber

#endif // SERVER_WORKER_THREAD_H


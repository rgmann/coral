#include <pthread.h>
#include <iostream>
#include "IThread.h"

using namespace liber::concurrency;

struct ThreadHandle
{
  pthread_t      thread;

  // POSIX thread attributes.
  pthread_attr_t attributes;
  bool bAttrsAllocated;
};

ui32 IThread::ournThreadId = 0;

//--------------------------------------------------------------------
IThread::IThread(const std::string& name)
: mpHandle(new ThreadHandle)
, mnThreadId(IThread::ournThreadId++)
, mName(name)
, mbRunning(false)
, mbShutdown(false)
{
  mpHandle->bAttrsAllocated = false;
}

//--------------------------------------------------------------------
IThread::~IThread()
{
  if (mbRunning)
  {
    cancel(false);
  }

  if (mpHandle->bAttrsAllocated)
  {
    pthread_attr_destroy(&mpHandle->attributes);
  }

  if (mpHandle)
  {
    delete mpHandle;
    mpHandle = NULL;
  }
}

//--------------------------------------------------------------------
ui32 IThread::getID() const
{
  return mnThreadId;
}

//--------------------------------------------------------------------
const std::string& IThread::getName() const
{
  return mName;
}

//--------------------------------------------------------------------
bool IThread::launch()
{
  int lnStatus = -1;

  if (!mbRunning)
  {
    if (mpHandle->bAttrsAllocated)
    {
      pthread_attr_destroy(&mpHandle->attributes);
    }

    mbShutdown = false;

    pthread_attr_init(&mpHandle->attributes);
    pthread_attr_setdetachstate(&mpHandle->attributes,
                                PTHREAD_CREATE_JOINABLE);

    // Flag allocation of thread attributes so that we know it needs
    // to be deleted.
    mpHandle->bAttrsAllocated = true;

    lnStatus = pthread_create(&mpHandle->thread,
                              &mpHandle->attributes,
                              ThreadEntry,
                              this);

    if (lnStatus != 0)
    {
      std::cout << mName << ".launch failure: "
                << strerror(lnStatus)
                << std::endl;
    }
  }

  return (lnStatus == 0);
}

//-----------------------------------------------------------------------------
bool IThread::join()
{
  int lnStatus = 0;

  if (mbRunning)
  {
    // Signal the shutdown.
    mbShutdown = true;

    lnStatus = pthread_join(mpHandle->thread, NULL);

    if (lnStatus != 0)
    {
      std::cout << mName << ".join failure: "
                << strerror(lnStatus)
                << std::endl;
    }
  }

  return (lnStatus == 0);
}

//-----------------------------------------------------------------------------
bool IThread::cancel(bool bJoin)
{
  bool lbSuccess = true;
  int  lnStatus = -1;

  if (mbRunning)
  {
    lnStatus = pthread_cancel(mpHandle->thread);

    // Signal the shutdown.
    mbShutdown = true;

    if (lnStatus != 0)
    {
      std::cout << mName << ".cancel failure: "
                << strerror(lnStatus)
                << std::endl;
    }

    lbSuccess = (lnStatus == 0);
  }

  if (bJoin && (lnStatus == 0))
  {
    lbSuccess = join();
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool IThread::isRunning() const
{
  return mbRunning;
}

//-----------------------------------------------------------------------------
void IThread::shutdown()
{
  if (mbRunning)
  {
    mbShutdown = true;
  }
}

//-----------------------------------------------------------------------------
void* IThread::ThreadEntry(void* pArg)
{
  IThread* lpThread = reinterpret_cast<IThread*>(pArg);

  // Signal that the thread has started.
  lpThread->mbRunning = true;

  // Push the cancellation handler onto the cleanup stack.  This handler
  // will always be executed regardless of whether the thread was cancelled
  // or joining of its own volition.
  pthread_cleanup_push(IThread::ExitHandler, pArg);

  // Run the thread routine.
  lpThread->run(lpThread->mbShutdown);

  pthread_cleanup_pop(pArg);

  pthread_exit(NULL);
}

//-----------------------------------------------------------------------------
void IThread::ExitHandler(void* pArg)
{
  IThread* lpThread = reinterpret_cast<IThread*>(pArg);

  // Signal that the thread is no longer running.
  lpThread->mbRunning = false;
}


#include "IThread.h"

//------------------------------------------------------------------------------
IThread::IThread(const std::string &name)
: mpThread(NULL)
, mName(name)
{
}

//------------------------------------------------------------------------------
IThread::~IThread()
{
   if (mpThread)
   {
      mpThread->stop();
      delete mpThread;
      mpThread = NULL;
   }
}

//------------------------------------------------------------------------------
bool IThread::start()
{
   if (mpThread) return false;
   
   mpThread = Thread::Create(ThreadFunc, this);
   
   return (mpThread != NULL);
}

//------------------------------------------------------------------------------
bool IThread::stop()
{
   return (mpThread && mpThread->stop());
}

//------------------------------------------------------------------------------
i32 IThread::getId() const
{
   return (mpThread != NULL) ? (i32)mpThread->getId() : -1;
}

//------------------------------------------------------------------------------
const std::string& IThread::getName() const
{
   return mName;
}

//------------------------------------------------------------------------------
void IThread::ThreadFunc(ThreadArg* pArg)
{
   ((IThread*)pArg->pUserData)->run(pArg->m_bStopSignal);
}

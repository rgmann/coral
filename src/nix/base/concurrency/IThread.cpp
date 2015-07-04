#include <iostream>
#include "IThread.h"

using namespace liber::concurrency;

ui32 IThread::ournThreadId = 0;

//--------------------------------------------------------------------
IThread::IThread(const std::string& name)
: mnThreadId(IThread::ournThreadId++)
, mName(name)
, mbRunning(false)
, mbShutdown(false)
{
}

//--------------------------------------------------------------------
IThread::~IThread()
{
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

//-----------------------------------------------------------------------------
bool IThread::launch()
{
  if ( !mbRunning )
  {
    mThread = boost::thread( &IThread::entry, this );
  }

  return true;
}

//-----------------------------------------------------------------------------
bool IThread::join()
{
  mThread.join();
  return true;
}

//-----------------------------------------------------------------------------
bool IThread::cancel(bool bJoin)
{
  mThread.interrupt();
  if ( bJoin ) mThread.join();

  return true;
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
void IThread::entry()
{
  mbRunning = true;

  run( mbShutdown );

  mbRunning = false;
}

#include <iostream>
#include "Log.h"
#include "IThread.h"

using namespace liber::concurrency;

ui32 IThread::ournThreadId = 0;

//--------------------------------------------------------------------
IThread::IThread( const std::string& name )
:  thread_id_  ( IThread::ournThreadId++ )
,  name_       ( name )
,  running_    ( false )
{
}

//--------------------------------------------------------------------
IThread::~IThread()
{
   thread_.interrupt();
   thread_.join();
}

//--------------------------------------------------------------------
ui32 IThread::getID() const
{
   return thread_id_;
}

//--------------------------------------------------------------------
const std::string& IThread::getName() const
{
   return name_;
}

//-----------------------------------------------------------------------------
bool IThread::launch()
{
   bool launch_success = false;

   if ( running_ == false )
   {
      thread_ = boost::thread( &IThread::entry, this );

      launch_success = true;
   }

   return launch_success;
}

//-----------------------------------------------------------------------------
void IThread::join()
{
   thread_.join();
}

//-----------------------------------------------------------------------------
void IThread::cancel( bool join )
{
   if ( running_ )
   {
      thread_.interrupt();

      if ( join )
      {
         thread_.join();
      }
   }
}

//-----------------------------------------------------------------------------
bool IThread::isRunning() const
{
   return running_;
}

//-----------------------------------------------------------------------------
void IThread::entry()
{
   running_ = true;

   run( boost::this_thread::interruption_requested() );

   running_ = false;
}

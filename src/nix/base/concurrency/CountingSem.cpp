// #include <pthread.h>
// #include <errno.h>
#include <string.h>
#include <stdio.h>
#include <boost/thread/thread_time.hpp>
#include <boost/thread/locks.hpp> 
#include "CountingSem.h"


//------------------------------------------------------------------------------
CountingSem::CountingSem(int initial)
:  mnCount( initial )
,  mnConsumerCount( 0 )
{
}

//------------------------------------------------------------------------------
CountingSem::~CountingSem()
{
}

//------------------------------------------------------------------------------
Sem::SemStatus CountingSem::take( i32 nTimeoutMs )
{
   bool lbGotSem = true;

   boost::mutex::scoped_lock guard( mLock );
   
   while ( mnCount == 0 )
   {
      if ( nTimeoutMs == Sem::SemWaitForever )
      {
         mCondition.wait( guard );
      }
      else
      {
         boost::system_time const timeout =
            boost::get_system_time() +
            boost::posix_time::milliseconds( nTimeoutMs );

         if ( ( lbGotSem = mCondition.timed_wait( guard, timeout ) ) )
         {
            break;
         }
      }
   }

   // Decrement the waiters count.
   if ( lbGotSem ) mnCount--;
   
   return lbGotSem ? SemAcquired : SemTimeout;
}

//------------------------------------------------------------------------------
Sem::SemStatus CountingSem::give()
{   
   boost::mutex::scoped_lock guard( mLock );

   // Increment the semaphore count to release one waiting thread.
   mnCount++;
   mCondition.notify_one();
   
   return SemOk;
}

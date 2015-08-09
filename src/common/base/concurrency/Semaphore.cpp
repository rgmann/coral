#include <boost/thread/thread_time.hpp>
#include <boost/thread/locks.hpp>
#include "Semaphore.h"

using namespace liber::thread;

//------------------------------------------------------------------------------
Semaphore::Semaphore( i32 start, i32 max )
:  maximum_count_( max )
, count_( start )
{
}

//------------------------------------------------------------------------------
Semaphore::~Semaphore()
{
}

//------------------------------------------------------------------------------
Semaphore::SemStatus Semaphore::take( i32 nTimeoutMs )
{
   bool recieved_notify = true;

   boost::mutex::scoped_lock guard( lock_ );
   
   while ( count_ == 0 )
   {
      if ( nTimeoutMs == Semaphore::SemWaitForever )
      {
         condition_.wait( guard );
      }
      else
      {
         boost::system_time const timeout =
            boost::get_system_time() +
            boost::posix_time::milliseconds( nTimeoutMs );

         recieved_notify = condition_.timed_wait( guard, timeout );
         break;
      }
   }

   // Decrement the waiters count.
   if ( recieved_notify )
   {
      count_--;
   }
   
   return recieved_notify ? SemAcquired : SemTimeout;
}

//------------------------------------------------------------------------------
Semaphore::SemStatus Semaphore::give()
{
   SemStatus status = SemOk;

   boost::mutex::scoped_lock guard( lock_ );

   if ( ( maximum_count_ == kUnlimited ) || ( count_ < maximum_count_ ) )
   {
      // Increment the semaphore count to release one waiting thread.
      count_++;
      condition_.notify_one();
   }
   else
   {
      status = SemError;
   }
   
   return status;
}

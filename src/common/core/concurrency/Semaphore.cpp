// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



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

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



#ifndef QUEUE_H
#define QUEUE_H

#include <string>
#include <queue>
#include <boost/thread/mutex.hpp>
#include "BaseTypes.h"
#include "CountingSem.h"

template <class T>
class Queue {
public:
   
   static const ui32 InfiniteQueue = 0;
   
   Queue( ui32 max_elements = InfiniteQueue );
   
   ~Queue();
      
   bool  push(const T &item, int nTimeoutMs = liber::thread::Semaphore::SemWaitForever);
   
   bool  pop(T &item, int nTimeoutMs = liber::thread::Semaphore::SemWaitForever);
   
   bool  peek(T &item);
   
   bool  isEmpty();
   
   ui32 size();
   
private:
   
   Queue(const Queue& src);
   
   Queue & operator=(const Queue &rhs);
   
private:
   
   ui32  max_elements_;
   bool  infinite_queue_;
   
   std::queue<T>  queue_;
   
   CountingSem  push_semaphore_;
   
   CountingSem  pop_semaphore_;

   boost::mutex queue_lock_;
};


//------------------------------------------------------------------------------
template <class T>
Queue<T>::Queue( ui32 max_elements )
: max_elements_( max_elements )
, infinite_queue_( ( max_elements_ == InfiniteQueue ) )
, push_semaphore_( 0 )
, pop_semaphore_( max_elements_ )
{
}

//------------------------------------------------------------------------------
template <class T>
Queue<T>::~Queue()
{
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::push( const T &item, int nTimeoutMs )
{
   bool can_push = true;

   if ( !infinite_queue_ )
   {
      can_push = (
         pop_semaphore_.take( nTimeoutMs ) != liber::thread::Semaphore::SemAcquired
      );
   }
      
   // The producer that acquires the push lock now needs to wait for room.
   if ( can_push )
   {
      {
         // Otherwise, the item can pushed onto the end of the queue.
         boost::mutex::scoped_lock guard( queue_lock_ );
         queue_.push( item );
      }
      
      // Post the push semaphore
      push_semaphore_.give();      
  }
   
   return can_push;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::pop(T &item, int nTimeoutMs)
{
   bool pop_success = false;
      
   if ( push_semaphore_.take( nTimeoutMs ) == liber::thread::Semaphore::SemAcquired )
   {
      {
         boost::mutex::scoped_lock guard( queue_lock_ );
         item = queue_.front();
         queue_.pop();
      }
      
      if ( !infinite_queue_ )
      {
         pop_semaphore_.give();
      }
      
      pop_success = true;
   }
   
   return pop_success;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::peek(T &item)
{
   boost::mutex::scoped_lock guard( queue_lock_ );
   bool peek_success = false;
   
   if ( queue_.empty() == false )
   {
      item = queue_.front();
      peek_success = true;
   }
   
   return peek_success;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::isEmpty()
{
   boost::mutex::scoped_lock guard( queue_lock_ );
   return queue_.empty();
}

//------------------------------------------------------------------------------
template <class T>
ui32 Queue<T>::size()
{
   boost::mutex::scoped_lock guard( queue_lock_ );
   return queue_.size();
}

#endif // QUEUE_H

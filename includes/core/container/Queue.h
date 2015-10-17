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

namespace coral {

///
/// Basic blocking template message queue class
///
template <class T>
class Queue {
public:
   
   static const ui32 kInfiniteQueue = 0;
   
   ///
   /// Construct a queue. The queue may be constructed to limit the number of
   /// elements, in which case the producer will block on inserting a new item
   /// into the queue.  If kInfiniteQueue is specified, the queue does not
   /// bound growth the producer is never blocked.
   ///
   /// @param  max_elements  Maximum number of elements that may be stored
   ///                       before the producer blocks on adding an item, or
   ///                       kInfiniteQueue.
   ///
   Queue( ui32 max_elements = kInfiniteQueue );
   
   ///
   /// Destructor
   ///
   ~Queue();
      
   ///
   /// Add one item to the end of the queue. If the queue capacity is bounder,
   /// the caller is blocked until at least one item is popped from the queue,
   /// or the timeout expires. If the queue size is unbounded, the timeout is
   /// ignored and push always returns true.
   ///
   /// @param  item  Item to add to the queue
   /// @param  timeout_ms  Maximum time, in milliseconds, to wait if the queue
   ///                     capacity is bounded.
   /// @return bool  True if the item was added to the queue; false on timeout.
   ///
   bool push( const T& item, i32 timeout_ms = coral::thread::Semaphore::SemWaitForever );
   
   ///
   /// Remove one item from the head of the queue. If the queue is empty, the
   /// caller is blocked until at least one item is available, or timeout
   /// period expires.
   ///
   /// @param  item  Item popped from the queue (only valid if pop returns true)
   /// @param  timeout_ms  Timeout period for the pop operation in milliseconds
   /// @return bool  True if an item is successfully popped; false on timeout.
   ///
   bool pop( T& item, i32 timeout_ms = coral::thread::Semaphore::SemWaitForever );

   ///
   /// Test whether the queue is empty.
   ///
   /// @return bool  True if the queue is currently empty; false otherwise.
   ///
   bool  isEmpty();

   ///
   /// Get the number of elements currently in the queue.
   ///
   /// @return ui32  Number of elements currently in the queue
   ///
   ui32 size();


private:
   
   ///
   /// Prevent copying
   ///
   Queue( const Queue& other );
   Queue& operator=( const Queue& other );
   
private:
   
   ui32  max_elements_;
   bool  infinite_queue_;
   
   std::queue<T>  queue_;
   
   thread::CountingSem  push_semaphore_;
   
   thread::CountingSem  pop_semaphore_;

   boost::mutex queue_lock_;
};


//------------------------------------------------------------------------------
template <class T>
Queue<T>::Queue( ui32 max_elements )
: max_elements_( max_elements )
, infinite_queue_( ( max_elements_ == kInfiniteQueue ) )
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
bool Queue<T>::push( const T &item, int timeout_ms )
{
   bool can_push = true;

   if ( infinite_queue_ == false )
   {
      can_push = (
         pop_semaphore_.take( timeout_ms ) != coral::thread::Semaphore::SemAcquired
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
bool Queue<T>::pop(T &item, int timeout_ms)
{
   bool pop_success = false;
      
   if ( push_semaphore_.take( timeout_ms ) == coral::thread::Semaphore::SemAcquired )
   {
      {
         boost::mutex::scoped_lock guard( queue_lock_ );
         item = queue_.front();
         queue_.pop();
      }
      
      if ( infinite_queue_ == false )
      {
         pop_semaphore_.give();
      }
      
      pop_success = true;
   }
   
   return pop_success;
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

} // end namespace coral

#endif // QUEUE_H

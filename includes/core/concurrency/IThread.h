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



#ifndef ITHREAD_H
#define ITHREAD_H

#include <boost/thread.hpp>
#include "BaseTypes.h"

namespace coral {
namespace thread {

// Abstract Implementable Thread
class IThread {
public:
   
   IThread( const std::string& name );

   virtual ~IThread();

   //
   // Launch the thread.
   //
   // @return False if the thread failed to launch or has already been
   //         launched; true otherwise.
   //
   virtual bool launch();

   //
   // Signal the thread to stop and wait for it to join.
   //
   // @return True if the thread successfully joins or has already joined;
   //         False otherwise.
   //
   void join();

   //
   // Cancel the thread and, optionally, wait for it to join.
   //
   // @param bJoin  Wait for the thread to join if true
   // @return True if the thread is successfully cancelled and, if enabled,
   //         joined.
   //
   void cancel( bool join = true );

   //
   // Each thread within a process receives a "unique" ID at instantiation time.
   //
   // @return Thread ID
   //
   ui32 getID() const;

   //
   // Get the thread name.
   //
   // @return Thread name string
   //
   const std::string& getName() const;

protected:

   //
   // Derived classes must implement the run method.
   //
   // @param bShutdown  Shutdown flag so that thread can respcond to a join
   //                   request.  Threads are configured to be cancellable,
   //                   but checking bShutdown allows for a more deterministic
   //                   exit if run() has multiple cancellation points.
   //
   virtual void run( const bool &shutdown ) = 0;

   bool isRunning() const;


private:

   void entry();


private:

   // Next thread ID.
   static ui32 ournThreadId;

   // Process-unique thread ID
   ui32 thread_id_;

   // Thread name
   std::string name_;

   boost::thread thread_;

   // Flag indicating that the thread context is active.
   bool running_;

};

} // end namespace thread
} // end namespace coral

#endif // ITHREAD_H

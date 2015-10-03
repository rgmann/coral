#ifndef ITHREAD_H
#define ITHREAD_H

#include <boost/thread.hpp>
#include "BaseTypes.h"

namespace liber {
namespace concurrency {

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

} // end namespace concurrency
} // end namespace liber

#endif // ITHREAD_H

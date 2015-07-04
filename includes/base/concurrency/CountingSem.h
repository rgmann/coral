#ifndef COUNTING_SEM_H
#define COUNTING_SEM_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "BaseTypes.h"
#include "Sem.h"

class CountingSem : public Sem
{
public:
   
   CountingSem( i32 nInitial );
   
   ~CountingSem();
   
   SemStatus take( i32 nTimeoutMs = SemWaitForever);
   
   SemStatus give();
   
private:
   
   // Current counting semaphore count.
   ui32 mnCount;
   
   // Number of cosumers waiting on the semaphore
   i32 mnConsumerCount;

   boost::mutex mLock;
   boost::condition_variable mCondition;
};

#endif // COUNTING_SEM_H
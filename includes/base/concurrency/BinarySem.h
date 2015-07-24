#ifndef BINARY_SEM_H
#define BINARY_SEM_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "BaseTypes.h"
#include "Sem.h"

class BinarySem : public Sem
{
public:
   
   enum BinarySemState
   {
      BinarySemUp,   // Can be taken
      BinarySemDown, // Can't be taken
   };
   
   BinarySem(BinarySemState start = BinarySemDown);
   
   ~BinarySem();
   
   SemStatus take(int nTimeoutMs = SemWaitForever);
   
   SemStatus give();
   
private:
   
   // Indicates binary state of the semaphore
   // bool  m_bIsTaken;
   int count_;

   boost::mutex lock_;
   boost::condition_variable condition_;
};

#endif // BINARY_SEM_H
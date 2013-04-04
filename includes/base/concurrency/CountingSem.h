#ifndef COUNTING_SEM_H
#define COUNTING_SEM_H

#include "Sem.h"

class CountingSem : public Sem
{
public:
   
   CountingSem(int initial);
   
   ~CountingSem();
   
   SemStatus take(int nTimeoutMs = SemWaitForever);
   
   SemStatus give();
   
private:
   
   // Current counting semaphore count.
   unsigned int m_nCount;
   
   // Number of cosumers waiting on the semaphore
   int m_nConsumerCount;
};

#endif // COUNTING_SEM_H